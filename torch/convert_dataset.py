import csv
from datetime import datetime
from sys import argv

# 定义一个函数，将日期时间字符串转换为秒
def convert_to_seconds(date_time_str):
    # 将日期时间字符串转换为datetime对象
    date_time_obj = datetime.strptime(date_time_str, "%Y-%m-%d %H:%M:%S")
    # 将datetime对象转换为自1970年1月1日以来的秒数
    return int(date_time_obj.timestamp())

# 原始CSV文件名
input_filename = argv[1]
# 输出文本文件名
output_filename = "converted_data.txt"

# 读取CSV文件并转换数据
with open(input_filename, 'r') as csvfile, open(output_filename, 'w') as outputfile:
    csvreader = csv.reader(csvfile)
    index = 0
    pre_time  = 0

    # 遍历CSV文件的每一行数据
    for row in csvreader:
        # 将日期时间转换为秒
        seconds = convert_to_seconds(row[0])
        
        if index == 0:
            pre_time = seconds
        else:
            # 将频率转换为整数
            frequency = int(row[1])
            # 将数据写入文本文件，格式为 "秒数 频率"
            delta_time = seconds - pre_time
            outputfile.write(f"{delta_time} {frequency}\n")
        index = index + 1

print(f"数据已转换并保存至 {output_filename}")