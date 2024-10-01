#pragma one
#include "db/version_edit.h"
#include <unordered_map>
#include <vector>

#include "leveldb/table.h"

namespace leveldb {

using ClusterLRData = std::pair<std::vector<double>, std::vector<double>>;

struct Cluster {
  uint64_t center_level;
  uint64_t center_access_time;

  double a;
  double b;
};

class SSTableWorth {
 public:
  SSTableWorth();

  bool IsWorth(uint64_t table_id, uint64_t next_period_time, int bf_number,
               int bits_per_key);

  void AddSSTable(uint64_t table_id, uint64_t level, uint64_t access_time);

  void ClusterSSTable(std::vector<FileMetaData*>* files);

 private:
  std::vector<Cluster> clusters_;
  std::unordered_map<uint64_t, int> table_to_cluster_;
  std::unordered_map<uint64_t, std::vector<uint64_t>> features_;

  void DoCluster(std::vector<FileMetaData*>* files);

  std::vector<ClusterLRData> DoAverage(std::vector<FileMetaData*>* files);

  void DoLineRegression(std::vector<ClusterLRData> data);

  std::vector<uint64_t> getTableID(std::vector<FileMetaData*>* files);

  void DoOneCluster(const std::vector<std::vector<uint64_t>>& data,
                    const std::vector<uint64_t>& table_id, int cluster_number);

  std::vector<std::vector<FileMetaData*>> TakePartOfFiles(
      std::vector<FileMetaData*>* files);

  std::vector<std::vector<uint64_t>> getFeatures(
      std::vector<FileMetaData*>* files);
};
}  // namespace leveldb