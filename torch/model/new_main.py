import numpy as np
import pandas as pd
import torch
from torch import nn
import model
import early_stopping
from torch.autograd import Variable
import os

input_len = 100
criterion = nn.MSELoss()
early_stopping = early_stopping.EarlyStopping(patience=3)
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
current_path = os.getcwd()
# todo, use a val
path = current_path + '/../dataset'
lr = 1e-2

def load_data(fileid):
    data_csv = pd.read_csv(path+f'/{fileid}.csv', usecols=[1])
    return data_csv

def create_dataset(dataset, look_back=4):
    dataX, dataY = [], []
    for i in range(len(dataset) - look_back):
        a = dataset[i:(i + look_back)] # 从0开始划分出一个长度为2的窗口
        dataX.append(a) # 放入要训练的值
        dataY.append(dataset[i + look_back]) # 放入要预测的值
    return np.array(dataX), np.array(dataY)


def normalization_data(data):
    # 数据预处理
    data = data.dropna()
    dataset = data.values
    dataset = dataset.astype('float32')
    value_max = np.max(dataset)
    value_min = np.min(dataset)
    scalar = value_max - value_min
    dataset = list(map(lambda x: x / scalar, dataset))
    return dataset, value_max, value_min

def split_data(datacsv, train_ratio, valid_ratio, test_ratio):
    dataset, value_max, value_min = normalization_data(datacsv)
    data_X, data_Y = create_dataset(dataset, input_len)

    # 生成随机排列的索引
    indices = np.random.permutation(len(data_X))

    # 计算每个部分的大小
    train_size = int(len(data_X) * train_ratio)
    valid_size = int(len(data_X) * valid_ratio)

    # 确定数据集的起始和结束索引
    train_end = train_size
    valid_end = train_size + valid_size

    # 划分数据集
    train_indices = indices[:train_end]
    valid_indices = indices[train_end:valid_end]
    test_indices = indices[valid_end:]

    # 划分数据
    train_X = data_X[train_indices]
    train_Y = data_Y[train_indices]

    valid_X = data_X[valid_indices]
    valid_Y = data_Y[valid_indices]

    test_X = data_X[test_indices]
    test_Y = data_Y[test_indices]

    # 训练train_X和train_Y的关系
    # 输入test_X，得到prev_Y，计算prev_Y和train_Y的区别
    train_X = train_X.reshape(-1, 1, input_len) #-1 代表不变
    train_Y = train_Y.reshape(-1, 1, 1)

    valid_X = valid_X.reshape(-1, 1, input_len)
    valid_Y = valid_Y.reshape(-1,1,1)

    test_X = test_X.reshape(-1, 1, input_len) # Y是要预测的
    test_Y = test_Y.reshape(-1,1,1)

    train_x = torch.from_numpy(train_X)
    train_y = torch.from_numpy(train_Y)

    valid_x = torch.from_numpy(valid_X)
    valid_y = torch.from_numpy(valid_Y)

    test_x = torch.from_numpy(test_X)
    test_y = torch.from_numpy(test_Y)

    train_x = torch.tensor(train_x, dtype=torch.float32, device=device).clone().detach()
    train_y = torch.tensor(train_y, dtype=torch.float32, device=device).clone().detach()

    valid_x = torch.tensor(valid_x, dtype=torch.float32, device=device).clone().detach()
    valid_y = torch.tensor(valid_y, dtype=torch.float32, device=device).clone().detach()

    test_x = torch.tensor(test_x, dtype=torch.float32, device=device).clone().detach()
    test_y = torch.tensor(test_y, dtype=torch.float32, device=device).clone().detach()

    return train_x, train_y, valid_x, valid_y, test_x, test_y, value_max, value_min

def pre_data(file_id):
    data_csv = load_data(file_id)
    return split_data(data_csv, 0.8,0,0.2)

def train_model(net, train_x, train_y, valid_x, valid_y):
    net.train()
    from torch.optim.lr_scheduler import CosineAnnealingLR
    optimizer = torch.optim.Adam(net.parameters(), lr=lr)
    scheduler = CosineAnnealingLR(optimizer, T_max=5000)  # 在这里设置余弦退火调度器的最大迭代次数

    # 开始训练
    for e in range(5000):
        var_x = Variable(train_x)
        var_y = Variable(train_y)
        # 前向传播
        out = net(var_x)
        loss = criterion(out, var_y)
        # 反向传播
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        scheduler.step()  # 在每个epoch结束后更新学习率
        if (e + 1) % 100 == 0: # 每 100 次输出结果
            print('Epoch: {}, Loss: {:.5f}'.format(e + 1, loss.data))

        if len(valid_x) > 0 :
            valid_var_x = Variable(valid_x)
            valid_var_y = Variable(valid_y)
            valid_output = net(valid_var_x)
            valid_loss = criterion(valid_output, valid_var_y)
            if (e + 1) % 100 == 0: # 每 100 次输出结果
                #print('valid Epoch: {}, Loss: {:.5f}'.format(e + 1, valid_loss.data))

                early_stopping(valid_loss)
                if early_stopping.early_stop:
                    print("Early stopping")
                    break

def eavl_model(net, test_x, test_y, value_max, value_min):
    from thop import profile
    input = (torch.randn(1, 1, input_len).to(device), )
    flops, params = profile(net, inputs = input)
    print('FLOPs = ' + str(flops/1000**3) + 'G')
    print('Params = ' + str(params/1000**2) + 'M')

    net = net.eval() # 转换成测试模式

    import time
    start_time = time.perf_counter()
    pred_test = net(test_x) # 测试集的预测结果
    end_time = time.perf_counter()

    print(end_time - start_time)
    # pred_test = pred_test * (value_max - value_min) + value_min
    # test_y    = test_y *   (value_max - value_min) + value_min

    # 计算损失
    loss_value = criterion(pred_test, test_y)  # 计算损失值
    print('Loss:', loss_value.item())  # 打印损失值
    # pred_test = pred_test.cpu().data.numpy()
    # test_y = test_y.cpu().data.numpy()

    #diff_y = pred_test - test_y
    #print('loss', np.mean(diff_y ** 2))

    import time
    num_iterations = 100
    total_time = 0
    for _ in range(num_iterations):
        start_time = time.time()
        with torch.no_grad():
            _ = net(test_x)
        end_time = time.time()
        total_time += end_time - start_time

    average_inference_time = total_time / num_iterations
    print(f'Average inference time: {average_inference_time:.5f} seconds')

def save(net, name, set_id):
    mod_dir = '.'
    model_data = Variable(train_x)
    traced_script_module = torch.jit.trace(net, model_data)
    name_string = ["rnn", "lstm", "gru"]
    model_path = f'{mod_dir}/{name_string[name]}_{set_id}_net.pt'  # Include name and set_id in the model path
    traced_script_module.save(model_path)

rnn = model.rnn_reg(input_len, 4).to(device)
lstm = model.lstm_reg(input_len, 4).to(device)
gru = model.gru_reg(input_len, 4).to(device)
transformer = model.TransformerReg(input_len, 4).to(device)

model_set = [rnn, lstm]
set = [1]


for index in set:
    train_x, train_y, valid_x, valid_y, test_x, test_y, value_max, value_min = pre_data(index)
    for mode_index, m in enumerate(model_set):
        train_model(m, train_x, train_y, valid_x, valid_y)
        eavl_model(m, test_x, test_y, value_max, value_min)
        save(m, mode_index, index)