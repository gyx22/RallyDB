#include "util/sstable_worth.h"

#include <limits>

#include "util/k_means.h"
#include "util/lr.h"

namespace leveldb {
double get_distance(uint64_t level, uint64_t access_time, Cluster cluster) {
  double delta_level = sqrt(level - cluster.center_level);
  double delta_access_time = sqrt(access_time - cluster.center_access_time);

  return pow(delta_access_time + delta_level, 0.5);
}

SSTableWorth::SSTableWorth() {
  clusters_.resize(7);
  for (int i = 0; i < clusters_.size(); i++) {
    clusters_[i].a = 0;
    clusters_[i].b = 0;
    clusters_[i].center_level = 0;
    clusters_[i].center_access_time = 0;
  }
}

bool SSTableWorth::IsWorth(uint64_t table_id, uint64_t next_period_time,
                           int bf_number, int bits_per_key) {
  if (!table_to_cluster_.count(table_id)) {
    return true;
  }

  int cluster_index = table_to_cluster_[table_id];
  Cluster cluster = clusters_[cluster_index];

  uint64_t next_sstable_time = cluster.a * next_period_time + cluster.b;
  double overhead = next_sstable_time * pow(0.6185, bits_per_key * bf_number) *
                    (1 - pow(0.6185, bits_per_key));

  // todo: add option
  return overhead <= 12;
}

void SSTableWorth::AddSSTable(uint64_t table_id, uint64_t level,
                              uint64_t access_time) {
  float min_dist = std::numeric_limits<float>::max();
  int min_index = 0;
  for (int i = 0; i < clusters_.size(); i++) {
    float dist = get_distance(level, access_time, clusters_[i]);
    if (dist < min_dist) {
      min_dist = dist;
      min_index = i;
    }
  }

  table_to_cluster_[table_id] = min_index;
}

std::vector<uint64_t> SSTableWorth::getTableID(
    std::vector<FileMetaData*>* files) {
  // get <table id, feature>, save table id
  std::vector<uint64_t> table_id;
  for (int i = 0; i < config::kNumLevels; i++) {
    std::vector<FileMetaData*> file_number = files[i];
    for (int j = 0; j < file_number.size(); j++) {
      table_id.push_back(file_number[j]->number);
    }
  }

  return table_id;
}

std::vector<std::vector<uint64_t>> SSTableWorth::getFeatures(
    std::vector<FileMetaData*>* files) {
  std::vector<std::vector<uint64_t>> features;
  for (int i = 0; i < config::kNumLevels; i++) {
    std::vector<FileMetaData*> file_number = files[i];
    for (int j = 0; j < file_number.size(); j++) {
      std::vector<uint64_t> f;
      f.push_back(i);
      f.push_back(file_number[j]->point_query_time_all);
      features.push_back(f);
    }
  }

  return features;
}

// init cluster center
// and table_id to cluster id mapping
void SSTableWorth::DoOneCluster(
    const std::vector<std::vector<uint64_t>>&
        data,  // feature(level, access time) std::vector<uint64_t> in every
               // sstable
    const std::vector<uint64_t>& table_id, int cluster_number) {
  if (data.empty()) {
    return;
  }
  KMEANS<uint64_t> kms(cluster_number);
  kms.loadDataSet(data);
  kms.randCent();
  kms.kmeans();

  // save table id to cluster id relateship
  std::vector<uint64_t> cluster_id = kms.getIndex();
  for (int i = 0; i < table_id.size(); i++) {
    table_to_cluster_[table_id[i]] = cluster_id[i];
  }

  // get center pointer
  std::vector<std::vector<uint64_t>> cluster_center = kms.getCentroids();
  assert(cluster_center.size() == clusters_.size());
  for (int i = 0; i < cluster_center.size(); i++) {
    clusters_[i].center_level = cluster_center[i][0];
    clusters_[i].center_access_time = cluster_center[i][1];
  }
}

void SSTableWorth::DoCluster(std::vector<FileMetaData*>* files) {
  std::vector<std::vector<uint64_t>> features = getFeatures(files);
  std::vector<uint64_t> table_id = getTableID(files);
  DoOneCluster(features, table_id, 7);
}

std::vector<std::vector<FileMetaData*>> SSTableWorth::TakePartOfFiles(
    std::vector<FileMetaData*>* files) {
  std::vector<std::vector<FileMetaData*>> result(7);
  for (int level = 0; level < config::kNumLevels; level++) {
    std::vector<FileMetaData*> d = files[level];
    for (int index = 0; index < d.size(); index++) {
      // get table's cluster id
      uint64_t number_id = d[index]->number;
      if (table_to_cluster_.count(number_id)) {
        continue;
      }
      uint64_t cluster_id = table_to_cluster_[number_id];
      result[cluster_id].push_back(d[index]);
    }
  }
  return result;
}

std::vector<ClusterLRData> SSTableWorth::DoAverage(
    std::vector<FileMetaData*>* files) {
  std::vector<ClusterLRData> result;
  std::vector<std::vector<FileMetaData*>> groups = TakePartOfFiles(files);
  for (int i = 0; i < groups.size(); i++) {
    std::vector<FileMetaData*> group = groups[i];
    if(group.empty()){
      continue;
    }
    // 先获得x，和max index
    int max_length = 0;
    int index = 0;
    for (int a = 0; a < group.size(); a++) {
      size_t s = group[a]->history_time.size();
      if (s > max_length) {
        max_length = s;
        index = a;
      }
    }
    
    // 获得LSM-tree时间片的查询频率
    std::vector<double> x;
    std::vector<std::pair<uint64_t, uint64_t>> history_time = group[index]->history_time;
    for (int b = 0; b < history_time.size(); b++) {
      x.push_back(history_time[b].first);
    }

    // 获得时间片下，group内各个SSTable的时间片的查询平均，不存在的当作0，并不计入平均
    std::vector<double> y;
    for(int d = max_length; d >= 0; d--){
      uint64_t sum = 0;
      uint64_t number = 0;
      for (int c = 0; c < group.size(); c++) {
        FileMetaData* meta_data = group[c];
        std::vector<std::pair<uint64_t, uint64_t>> history_time = meta_data->history_time;
        size_t all_size = history_time.size();
        if(all_size - d - 1 < 0){
          continue;
        }
        std::pair<uint64_t, uint64_t>  r = history_time[all_size - d - 1];
        sum += r.second;
        number++;
      }

      y.push_back((double)(sum/number));
    }

    ClusterLRData lrdata(x, y);
    result.push_back(lrdata);
  }

  return {};
}

void SSTableWorth::DoLineRegression(std::vector<ClusterLRData> data) {
  for (int i = 0; i < data.size(); i++) {
    ClusterLRData d = data[i];
    LeastSquare l(d.first, d.second);

    clusters_[i].a = l.return_a();
    clusters_[i].b = l.return_b();
  }
}

// take part of it, cluster and line regiress
void SSTableWorth::ClusterSSTable(std::vector<FileMetaData*>* files) {
  DoCluster(files);
  std::vector<ClusterLRData> d = DoAverage(files);
  DoLineRegression(d);
}
}  // namespace leveldb