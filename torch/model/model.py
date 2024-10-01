
from torch import nn
import numpy as np


class lstm_reg(nn.Module):
    # 网络结构定义
    # 参数传入
    def __init__(self, input_size, hidden_size, output_size=1, num_layers=2):
        super(lstm_reg, self).__init__()
        
        self.rnn = nn.LSTM(input_size, hidden_size, num_layers) # rnn
        self.reg = nn.Linear(hidden_size, output_size) # 回归
        
    def forward(self, x): # 该模块的推理过程
        x, _ = self.rnn(x) # (seq, batch, hidden)
        s, b, h = x.shape
        x = x.view(s*b, h) # 转换成线性层的输入格式
        x = self.reg(x) # 输入线性层
        x = x.view(s, b, -1)
        return x

class rnn_reg(nn.Module):
    # 网络结构定义
    # 参数传入
    def __init__(self, input_size, hidden_size, output_size=1, num_layers=2):
        super(rnn_reg, self).__init__()
        
        self.rnn = nn.RNN(input_size, hidden_size, num_layers) # RNN
        self.reg = nn.Linear(hidden_size, output_size) # 回归
        
    def forward(self, x): # 该模块的推理过程
        x, _ = self.rnn(x) # (seq, batch, hidden)
        s, b, h = x.shape
        x = x.view(s*b, h) # 转换成线性层的输入格式
        x = self.reg(x) # 输入线性层
        x = x.view(s, b, -1)
        return x
    

class gru_reg(nn.Module):
    def __init__(self, input_size, hidden_size, output_size=1, num_layers=2):
        super(gru_reg, self).__init__()
        
        self.gru = nn.GRU(input_size, hidden_size, num_layers)  # GRU
        self.reg = nn.Linear(hidden_size, output_size)  # Regression layer
        
    def forward(self, x):
        x, _ = self.gru(x)  # (seq, batch, hidden)
        s, b, h = x.shape
        x = x.view(s * b, h)  # Reshape for linear layer input
        x = self.reg(x)  # Linear layer
        x = x.view(s, b, -1)
        return x
    


import torch
import torch.nn as nn
import torch.nn.functional as F

class TransformerReg(nn.Module):
    def __init__(self, input_size, hidden_size, output_size=1, num_layers=2):
        super(TransformerReg, self).__init__()
        
        self.embedding = nn.Embedding(input_size, hidden_size)
        self.positional_encoding = PositionalEncoding(hidden_size)  # You'll need to define this class
        self.transformer = nn.Transformer(d_model=hidden_size, nhead=2, num_encoder_layers=num_layers)
        self.reg = nn.Linear(hidden_size, output_size)  # Regression layer
        
    def forward(self, x):
        embedded = self.embedding(x)
        embedded = embedded * math.sqrt(self.hidden_size)  # Scale the embedding
        embedded = self.positional_encoding(embedded)  # Add positional encoding
        x = self.transformer(embedded, embedded)  # Apply transformer
        x = self.reg(x)  # Linear layer
        return x
    
class PositionalEncoding(nn.Module):
    def __init__(self, d_model, dropout=0.1, max_len=5000):
        super(PositionalEncoding, self).__init__()
        self.dropout = nn.Dropout(p=dropout)

        position = torch.arange(max_len).unsqueeze(1)
        div_term = torch.exp(torch.arange(0, d_model, 2) * -(np.log(10000.0) / d_model))
        pe = torch.zeros(max_len, d_model)
        pe[:, 0::2] = torch.sin(position * div_term)
        pe[:, 1::2] = torch.cos(position * div_term)
        pe = pe.unsqueeze(0).transpose(0, 1)
        self.register_buffer('pe', pe)

    def forward(self, x):
        x = x + self.pe[:x.size(0), :]
        return self.dropout(x)