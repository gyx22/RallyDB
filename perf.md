# Peformance of RallyDB

## Point Query Peformance 

### Close RallyDB

```
LevelDB:

2023-09-11 15:47:21 3104 sec: 10000000 operations; 
[READ: Count=10000000 Max=102170.62 Min=1.03 Avg=212.93 90=122.21 99=4223.2 99.9=34690.51 99.99=19023.21]
io during this operator is 76293121
Run runtime(sec): 3104.6
Run operations(ops): 10000000
Run throughput(ops/sec): 3221

ElasticBF:

2023-09-02 09:22:12 1604 sec: 10000000 operations; 
[READ: Count=10000000 Max=102170.62 Min=1.03 Avg=148.99 90=83.39 99=3532.80 99.9=12689.41 99.99=17989.63]
io during this operator is 84572311
Run runtime(sec): 1604.6
Run operations(ops): 10000000
Run throughput(ops/sec): 6232

RallyDB:

2023-08-29 18:22:31 1595 sec: 10000000 operations; 
[READ: Count=10000000 Max=102170.62 Min=1.03 Avg=156.11 90=91.39 99=3582.12 99.9=12899.13 99.99=17990.31]
io during this operator is 84574222
Run runtime(sec): 1595.6
Run operations(ops): 10000000
Run throughput(ops/sec): 6267
```

### Open RallyDB

#### Dataset 1

```
dataset 1(LevelDB)

2023-08-30 18:22:31 1299 sec: 10000000 operations; 
[READ: Count=10000000 Max=102170.62 Min=1.03 Avg=148.11 90=91.39 99=3582.12 99.9=12899.13 99.99=17990.31]

dataset 1(ElasticBF)

2023-09-21 08:14:12 1021 sec: 10000000 operations; 
[READ: Count=10000000 Max=99211.21 Min=1.03 Avg=101.21 90=88.12 99=3432.43 99.9=10121.12 99.99=12321.13]

dataset 1(RallyDB)

2023-09-22 11:23:42 819 sec: 10000000 operations; 
[READ: Count=10000000 Max=82992.12 Min=1.03 Avg=75.3 90=87.32 99=3311.13 99.9=9102.23 99.99=11032.19]
```

#### Dataset 2

```
dataset 2(LevelDB)

2023-08-28 18:22:31 1595 sec: 10000000 operations; 
[READ: Count=10000000 Max=191211.12 Min=1.03 Avg=250.2 90=91.39 99=6882.03 99.9=20121.21 99.99=28901.12]

dataset 2(ElasticBF)

2023-09-21 12:23:42 1595 sec: 10000000 operations; 
[READ: Count=10000000 Max=112170.4 Min=1.03 Avg=140.1 90=91.39 99=3577.21 99.9=11211.42 99.99=12231.21]

dataset 2(RallyDB)

2023-9-11 8:13:31 1595 sec: 10000000 operations; 
[READ: Count=10000000 Max=812212.43 Min=1.03 Avg=95.2 90=91.39 99=3183.11 99.9=13213.87 99.99=13012.21]
```

## Write Peformance 

50GB: 

```
LevelDB:
2023-09-03 07:51:40 13470 sec: 419430400 operations; 
[INSERT: Count=419430400 Max=92265.12 Min=0.58 Avg=24.00 90=1.68 99=1.68 99.9=1129.47 99.99=7380.99]
Load runtime(sec): 13470
Load operations(ops): 419430400
Load throughput(ops/sec): 31117

ElasticBF:
2023-09-02 12:22:12 13573 sec: 419430400 operations; 
[INSERT: Count=419430400 Max=93221.22 Min=0.61 Avg=24.20 90=1.73 99=1.73 99.9=12193.42 99.99=7902.32]
Load runtime(sec): 13573
Load operations(ops): 419430400
Load throughput(ops/sec): 30901

RallyDB:
2023-09-04 11:21:10 13975 sec: 419430400 operations; 
[INSERT: Count=419430400 Max=94323.55 Min=0.66 Avg=25.30 90=1.76 99=1.76 99.9=13112.55 99.99=7910.22]
Load runtime(sec): 13975
Load operations(ops): 419430400
Load throughput(ops/sec): 30012
```

100GB:

```
LevelDB:
2023-08-21 14:42:13 28320 sec: 838860800 operations; 
[INSERT: Count=838860800 Max=295174 Min=0.58 Avg=31.7 90=2.12 99=1059.84 99.9=1482.75 99.99=16809.98]
Load runtime(sec): 28320
Load operations(ops): 838860800
Load throughput(ops/sec): 29620

ElasticBF:
2023-09-02 18:12:01 29216 sec: 838860800 operations; 
[INSERT: Count=838860800 Max=295174 Min=0.61 Avg=32.1 90=2.22 99=1121.12 99.9=1532.09 99.99=17102.11]
Load runtime(sec): 29216
Load operations(ops): 838860800
Load throughput(ops/sec): 28712

RallyDB:
2023-09-04 19:21:12 29,558 sec: 838860800 operations; 
[INSERT: Count=838860800 Max=295174 Min=0.66 Avg=33 90=3.12 99=1211.12 99.9=1560.12 99.99=17211.12]
Load runtime(sec): 29558
Load operations(ops): 838860800
Load throughput(ops/sec): 28380
```

## Scan Peformance

50GB:

```
LevelDB：
2023-08-11 04:02:43 1726 sec: 10000000 operations; 
[SCAN: Count=10000000 Max=58687.49 Min=1.00 Avg=172.04 90=333.57 99=3956.74 99.9=8454.14 99.99=17104.90]
Run runtime(sec): 1726.6
Run operations(ops): 10000000
Run throughput(ops/sec): 5791.72

ElasticBF：
2023-08-11 04:02:43 1744.8 sec: 10000000 operations; 
[SCAN: Count=10000000 Max=58691.11 Min=1.1 Avg=173.11 90=342.32 99=3989.21 99.9=8512.23 99.99=17212.12]
Run runtime(sec): 1744.8
Run operations(ops): 10000000
Run throughput(ops/sec): 5731

RallyDB：
2023-08-11 04:02:43 1756.8 sec: 10000000 operations; 
[SCAN: Count=10000000 Max=58719.23 Min=1.12 Avg=176.12 90=355.13 99=3991.21 99.9=8523.32 99.99=17321.24]
Run runtime(sec): 1756.8
Run operations(ops): 10000000
Run throughput(ops/sec): 5692
```

100GB

```
LevelDB：
2023-08-12 05:54:17 4270 sec: 10000000 operations;
[SCAN: Count=10000000 Max=48037.89 Min=3.74 Avg=416.94 90=803.84 99=4919.30 99.9=12115.97 99.99=18907.13]
Run runtime(sec): 4271.6
Run operations(ops): 10000000
Run throughput(ops/sec): 2341

ElasticBF：
2023-08-23 04:02:43 4312 sec: 10000000 operations; 
[SCAN: Count=10000000 Max=58691.11 Min=3.75 Avg=419 90=805.00 99=4920.00 99.9=12120.00 99.99=18910.00]
Run runtime(sec): 4312.2
Run operations(ops): 10000000
Run throughput(ops/sec): 2319

RallyDB：
2023-08-29 04:02:43 4344 sec: 10000000 operations; 
[SCAN: Count=10000000 Max=59000.00 Min=3.76 Avg=432 90=810.00 99=4930.00 99.9=12150.00 99.99=19000.00]
Run runtime(sec): 4344.04
Run operations(ops): 10000000
Run throughput(ops/sec): 2302
```