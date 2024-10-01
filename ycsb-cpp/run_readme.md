# cmake

run build.sh


# workload
Command line just:

```
./ycsb -run -db basic -P ../workloads/workload_elasticbf  -p threadcount=4 -p basic.silent=true -p limit.ops=4 -p limit.file=/home/wangtingzheng_21/system/RallyDB/torch/converted_data.txt -s

2024-04-24 21:17:43 0 sec: 0 operations;
2024-04-24 21:17:53 10 sec: 36 operations; [READ: Count=36 Max=2.63 Min=0.13 Avg=0.94 90=1.51 99=2.63 99.9=2.63 99.99=2.63]
2024-04-24 21:18:03 20 sec: 76 operations; [READ: Count=76 Max=2.96 Min=0.13 Avg=1.02 90=1.69 99=2.63 99.9=2.96 99.99=2.96]
2024-04-24 21:18:13 30 sec: 116 operations; [READ: Count=116 Max=2.96 Min=0.13 Avg=1.08 90=1.87 99=2.63 99.9=2.96 99.99=2.96]
2024-04-24 21:18:23 40 sec: 156 operations; [READ: Count=156 Max=5.27 Min=0.13 Avg=1.20 90=1.98 99=4.00 99.9=5.27 99.99=5.27]
2024-04-24 21:18:33 50 sec: 196 operations; [READ: Count=196 Max=6.29 Min=0.10 Avg=1.20 90=1.98 99=5.13 99.9=6.29 99.99=6.29]
2024-04-24 21:18:43 60 sec: 236 operations; [READ: Count=236 Max=6.61 Min=0.10 Avg=1.26 90=2.06 99=5.38 99.9=6.61 99.99=6.61]
2024-04-24 21:18:53 70 sec: 276 operations; [READ: Count=276 Max=6.61 Min=0.10 Avg=1.24 90=2.08 99=5.27 99.9=6.61 99.99=6.61]
2024-04-24 21:19:03 80 sec: 316 operations; [READ: Count=316 Max=6.61 Min=0.10 Avg=1.23 90=2.07 99=5.27 99.9=6.61 99.99=6.61]
2024-04-24 21:19:13 90 sec: 356 operations; [READ: Count=356 Max=6.61 Min=0.10 Avg=1.23 90=2.06 99=5.13 99.9=6.61 99.99=6.61]
2024-04-24 21:19:23 100 sec: 396 operations; [READ: Count=396 Max=6.61 Min=0.10 Avg=1.24 90=2.06 99=5.13 99.9=6.61 99.99=6.61]
2024-04-24 21:19:33 110 sec: 436 operations; [READ: Count=436 Max=6.61 Min=0.10 Avg=1.24 90=2.02 99=5.13 99.9=6.61 99.99=6.61]
2024-04-24 21:19:43 120 sec: 476 operations; [READ: Count=476 Max=6.61 Min=0.10 Avg=1.24 90=2.02 99=4.00 99.9=6.61 99.99=6.61]
2024-04-24 21:19:53 130 sec: 516 operations; [READ: Count=516 Max=6.61 Min=0.10 Avg=1.24 90=1.98 99=4.00 99.9=6.29 99.99=6.61]
2024-04-24 21:20:03 140 sec: 556 operations; [READ: Count=556 Max=6.61 Min=0.10 Avg=1.24 90=1.98 99=4.00 99.9=6.29 99.99=6.61]
2024-04-24 21:20:13 150 sec: 596 operations; [READ: Count=596 Max=6.61 Min=0.10 Avg=1.25 90=1.98 99=4.00 99.9=6.29 99.99=6.61]
```

```limit.ops=4``` stand for 0 times ops, about 3 ops/sec, close to 0

csv format:

```
"2016-11-29 21:10:00"
"2016-11-29 21:20:00","288"
"2016-11-29 21:30:00","292"
"2016-11-29 21:40:00","368"
"2016-11-29 21:50:00","335"
```
