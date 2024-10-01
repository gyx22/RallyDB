import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np

# 定义Transformer模型
class TransformerModel(nn.Module):
    def __init__(self, input_dim, output_dim, nhead, nhid, nlayers, max_seq_length):
        super(TransformerModel, self).__init__()
        
        self.model_type = 'Transformer'
        self.pos_encoder = PositionalEncoding(input_dim, max_seq_length)
        encoder_layers = nn.TransformerEncoderLayer(input_dim, nhead, nhid)
        self.transformer_encoder = nn.TransformerEncoder(encoder_layers, nlayers)
        self.encoder = nn.Linear(input_dim, nhid)
        self.decoder = nn.Linear(nhid, output_dim)

    def forward(self, src):
        src = self.pos_encoder(src)
        output = self.transformer_encoder(src)
        output = self.encoder(output)
        output = self.decoder(output)
        return output

# 定义位置编码层
class PositionalEncoding(nn.Module):
    def __init__(self, d_model, max_len=5000):
        super(PositionalEncoding, self).__init__()
        self.dropout = nn.Dropout(p=0.1)

        pe = torch.zeros(max_len, d_model)
        position = torch.arange(0, max_len, dtype=torch.float).unsqueeze(1)
        div_term = torch.exp(torch.arange(0, d_model, 2).float() * (-np.log(10000.0) / d_model))
        pe[:, 0::2] = torch.sin(position * div_term)
        pe[:, 1::2] = torch.cos(position * div_term)
        pe = pe.unsqueeze(0).transpose(0, 1)
        self.register_buffer('pe', pe)

    def forward(self, x):
        x = x + self.pe[:x.size(0), :]
        return self.dropout(x)


# 创建数据
input_dim = 4
output_dim = 1
nhead = 4
nhid = 32
nlayers = 3
max_seq_length = 100
model = TransformerModel(input_dim, output_dim, nhead, nhid, nlayers, max_seq_length)

# 使用模型进行预测
input_sequence = torch.rand(100, 1, input_dim)  # 输入序列长度为10
output = model(input_sequence)
print(output.shape)  # 输出形状为 (10, max_seq_length, output_dim)

from thop import profile
flops, params = profile(model, inputs = (input_sequence,))
print('FLOPs = ' + str(flops/1000**3) + 'G')
print('Params = ' + str(params/1000**2) + 'M')

import time
num_iterations = 100
total_time = 0
for _ in range(num_iterations):
    start_time = time.time()
    with torch.no_grad():
        _ = model(input_sequence)
    end_time = time.time()
    total_time += end_time - start_time

average_inference_time = total_time / num_iterations
print(f'Average inference time: {average_inference_time:.5f} seconds')
