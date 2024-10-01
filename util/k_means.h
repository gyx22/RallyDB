#ifndef STORAGE_LEVELDB_DB_K_MEANS_H_
#define STORAGE_LEVELDB_DB_K_MEANS_H_

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits.h>  //for INT_MIN INT_MAX
#include <map>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>  //for srand
#include <vector>

namespace leveldb {
using namespace std;

template <typename T>
class KMEANS {
 private:
  vector<vector<T>> dataSet;  // the data set
  vector<T> mmin, mmax;
  int colLen,
      rowLen;  // colLen:the dimension of vector;rowLen:the number of vectors
  int k;
  vector<vector<T>> centroids;
  typedef struct MinMax {
    T Min;
    T Max;
    MinMax(T min, T max) : Min(min), Max(max) {}
  } tMinMax;
  typedef struct Node {
    int minIndex;  // the index of each node
    double minDist;
    Node(int idx, double dist) : minIndex(idx), minDist(dist) {}
  } tNode;
  vector<tNode> clusterAssment;

  /*split line into numbers*/
  void split(char* buffer, vector<T>& vec) {
    char* p = strtok(buffer, " \t");
    while (p != NULL) {
      vec.push_back(atof(p));
      p = strtok(NULL, " ");
    }
  }
  tMinMax getMinMax(int idx) {
    T min, max;
    dataSet[0].at(idx) > dataSet[1].at(idx)
        ? (max = dataSet[0].at(idx), min = dataSet[1].at(idx))
        : (max = dataSet[1].at(idx), min = dataSet[0].at(idx));

    for (int i = 2; i < rowLen; i++) {
      if (dataSet[i].at(idx) < min)
        min = dataSet[i].at(idx);
      else if (dataSet[i].at(idx) > max)
        max = dataSet[i].at(idx);
      else
        continue;
    }

    tMinMax tminmax(min, max);
    return tminmax;
  }
  void setCentroids(tMinMax& tminmax, int idx) {
    T rangeIdx = tminmax.Max - tminmax.Min;
    for (int i = 0; i < k; i++) {
      /* generate float data between 0 and 1 */
      centroids[i].at(idx) =
          tminmax.Min + rangeIdx * (rand() / (double)RAND_MAX);
    }
  }
  void initClusterAssment() {
    tNode node(-1, -1);
    for (int i = 0; i < rowLen; i++) {
      clusterAssment.push_back(node);
    }
  }
  double distEclud(vector<T>& v1, vector<T>& v2) {
    T sum = 0;
    int size = v1.size();
    for (int i = 0; i < size; i++) {
      sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return sum;
  }

 public:
  KMEANS(int k) { this->k = k; }
  void loadDataSet(const vector<vector<T>>& dataSet) {
    this->dataSet = dataSet;
    // init colLen,rowLen
    colLen = dataSet[0].size();
    rowLen = dataSet.size();
  }
  void randCent() {
    // init centroids
    vector<T> vec(colLen, 0);
    for (int i = 0; i < k; i++) {
      centroids.push_back(vec);
    }

    // set values by column
    srand(time(NULL));
    for (int j = 0; j < colLen; j++) {
      tMinMax tminmax = getMinMax(j);
      setCentroids(tminmax, j);
    }
  }
  void print() {
    typename vector<vector<T>>::iterator it = dataSet.begin();
    typename vector<tNode>::iterator itt = clusterAssment.begin();
    for (int i = 0; i < rowLen; i++) {
      typename vector<T>::iterator it2 = (*it).begin();
      while (it2 != (*it).end()) {
        std::cout << *it2 << "\t";  // 使用 std::cout 打印数据点
        it2++;
      }
      std::cout << (*itt).minIndex << std::endl;  // 打印聚类索引并换行
      itt++;
      it++;
    }
  }
  void kmeans() {
    initClusterAssment();
    bool clusterChanged = true;
    // the termination condition can also be the loops less than	some
    // number such as 1000
    while (clusterChanged) {
      clusterChanged = false;
      // step one : find the nearest centroid of each point
      // cout<<"find the nearest centroid of each point : "<<endl;
      for (int i = 0; i < rowLen; i++) {
        int minIndex = -1;
        double minDist = INT_MAX;
        for (int j = 0; j < k; j++) {
          double distJI = distEclud(centroids[j], dataSet[i]);
          if (distJI < minDist) {
            minDist = distJI;
            minIndex = j;
          }
        }
        if (clusterAssment[i].minIndex != minIndex) {
          clusterChanged = true;
          clusterAssment[i].minIndex = minIndex;
          clusterAssment[i].minDist = minDist;
        }
      }

      // step two : update the centroids
      // cout<<"update the centroids:"<<endl;
      for (int cent = 0; cent < k; cent++) {
        vector<T> vec(colLen, 0);
        int cnt = 0;
        for (int i = 0; i < rowLen; i++) {
          if (clusterAssment[i].minIndex == cent) {
            ++cnt;
            // sum of two vectors
            for (int j = 0; j < colLen; j++) {
              vec[j] += dataSet[i].at(j);
            }
          }
        }

        // mean of the vector and update the centroids[cent]
        for (int i = 0; i < colLen; i++) {
          if (cnt != 0) vec[i] /= cnt;
          centroids[cent].at(i) = vec[i];
        }
      }  // for
      // print();//update the centroids
    }  // while

#if 0
	typename vector<tNode> :: iterator it = clusterAssment.begin();
	while( it!=clusterAssment.end() )
	{
		cout<<(*it).minIndex<<"\t"<<(*it).minDist<<endl;
		it++;	
	}
#endif
  }
  void print_center() {
    std::vector<std::vector<T>> center = getCentroids();
    for (int i = 0; i < center.size(); i++) {
      std::vector<T> one_center = center[i];
      for (int j = 0; j < one_center.size(); j++) {
        std::cout << one_center[i] << std::endl;
      }
    }
  }
  std::vector<uint64_t> getIndex() {
    typename vector<tNode>::iterator itt = clusterAssment.begin();
    std::vector<uint64_t> return_v;

    for (int i = 0; i < rowLen; i++) {
      return_v.push_back((*itt).minIndex);
      itt++;
    }

    return return_v;
  }

  std::vector<std::vector<T>> getCentroids() const { return centroids; }
};
}  // namespace leveldb

#endif