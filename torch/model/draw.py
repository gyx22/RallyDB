import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data_csv = pd.read_csv('/home/wangtingzheng_21/ai/point-query-forecaster/assets/data/7.csv', usecols=[1])


plt.plot(data_csv)

plt.title('数据集4', fontproperties='SimHei', size=20)
plt.xlabel("时间", fontproperties='SimHei', size=15)
plt.ylabel("查询频率", fontproperties='SimHei', size=15)
plt.savefig('/home/wangtingzheng_21/ai/point-query-forecaster/model/plot.png')