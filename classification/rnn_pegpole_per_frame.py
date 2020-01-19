
# coding: utf-8

# In[1]:

from __future__ import unicode_literals, print_function, division
from io import open
import glob
import os
import numpy as np
import sys

fw = open("perframe_srnn_taurus_results.txt", "a")

# XT = np.loadtxt("data/jigsaws/Suturing_X.txt", delimiter=" ", dtype='double')
# yT = np.loadtxt("data/jigsaws/Suturing_y.txt", delimiter=" ", dtype='double')
# print ("jigsaws/Suturing data")

# XT = np.loadtxt("data/pegpole/taurus_sim_kinematics_feature_X.txt", delimiter=" ", dtype='double')
# yT = np.loadtxt("data/pegpole/taurus_sim_kinematics_feature_y.txt", delimiter=" ", dtype='double')

XT = np.loadtxt("data/taurus_kinematics_feature_X.txt", delimiter=" ", dtype='double')
yT = np.loadtxt("data/taurus_kinematics_feature_y.txt", delimiter=" ", dtype='double')
print ("Taurus physical robot data")
# value normalization
from sklearn.preprocessing import scale
XT = scale( XT, axis=0, with_mean=True, with_std=True, copy=True )

# xm = XT.mean(axis=0)
# xs = XT.std(axis=0)
# print (xm)
# print(xs)
# fw.close()
# sys.exit()
# print (XT)
# print(type(XT))
print (len(XT))
print (len(XT[0]))

# print (yT)
# print(type(yT))
print (len(yT))


# In[2]:

import torch

constraints = {}
# constraints[0] = [] # not a valid surgeme, need to handle. for now just treat as dummy surgeme
# constraints[1] = [1, 5]
# constraints[2] = [2, 3, 6]
# constraints[3] = []
# constraints[4] = []
# constraints[5] = []
# constraints[6] = []
# constraints[7] = []
# constraints[8] = [8, 2, 3]
# constraints[9] = []
# constraints[10] = [10, 2, 6]
# constraints[11] = []

constraints[0] = [] # not a valid surgeme, need to handle. for now just treat as dummy surgeme
constraints[1] = []
constraints[2] = []
constraints[3] = []
constraints[4] = []
constraints[5] = []
constraints[6] = []
constraints[7] = []
constraints[8] = []
constraints[9] = []
constraints[10] = []
constraints[11] = []

# countExp = 20000
data = XT
data_y = yT
#print (data_y)
# print(type(data_y))
# print (len(data_y))
# print (len(data_y[0]))

x = torch.from_numpy(data)
y = torch.from_numpy(data_y)
# print (x.size())
# print (x[0])
# print (y.size())
# print (y[0])
# x = torch.Tensor(data[:-1])
# y = torch.Tensor(data[1:]).type(dtype)

# pegpole
surgemes = ["G0", "G1", "G2", "G3", "G4", "G5", "G6", "G7"]
all_categories = [0, 1, 2, 3, 4, 5, 6, 7]

# JIGSAWS
# surgemes = ["G0", "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11"]
# all_categories = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]

# all_categories = surgemes
n_categories = len(all_categories)

# constraint filter
constraint_filter = True
# constraint_filter = False
import torch.nn as nn

class RNN(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(RNN, self).__init__()

        self.hidden_size = hidden_size

        self.i2h = nn.Linear(input_size + hidden_size, hidden_size)
        self.i2o = nn.Linear(input_size + hidden_size, output_size)
        self.softmax = nn.LogSoftmax(dim=1)

    def forward(self, input, hidden):
#         print ("Getting input forward:")
#         print( input)
#         print ("Getting hidden forward:")
#         print(hidden)
        hidden = hidden.double()
        combined = torch.cat((input, hidden), 1)
#         print("combined")
#         print(combined)
#         print(combined.type())
#         print("combined convert ")
        combined = combined.float()
#         print(combined)
#         print(combined.type())
        hidden = self.i2h(combined)
        output = self.i2o(combined)
        output = self.softmax(output)
        return output, hidden

    def initHidden(self):
        return torch.zeros(1, self.hidden_size, dtype=torch.float64)


# In[4]:

# network config
n_hidden = 128
n_input = (x[0].size())[0]
# print (n_input)

rnn = RNN(n_input, n_hidden, n_categories)

input = (x[0].unsqueeze(0))
# print ("Input:")
# print (input)
# print (input.type())

hidden = torch.zeros(1, n_hidden, dtype=torch.float64)
# print("hidden")
# print (hidden)
# print (hidden.type())
output, next_hidden = rnn(input, hidden)

# print("Output")
# print (output)
# print (output.type())


# In[5]:

def categoryFromOutput(output):
    top_n, top_i = output.topk(1)
#     print (top_n)
#     print (top_i)
    category_i = top_i[0].item()
    return all_categories[category_i], category_i

print (categoryFromOutput(output))

# import random

# def randomChoice(l):
#     return l[random.randint(0, len(l) - 1)]

# def randomTrainingExample():
#     category = randomChoice(all_categories)
#     line = randomChoice(category_lines[category])
#     category_tensor = torch.tensor([all_categories.index(category)], dtype=torch.long)
#     line_tensor = lineToTensor(line)
#     return category, line, category_tensor, line_tensor


# In[6]:


# for i in range(10):
#     category, line, category_tensor, line_tensor = randomTrainingExample()
#     print('category =', category, '/ line =', line)
# # training data
# for i in ra
criterion = nn.NLLLoss()

learning_rate = 0.0005 # if too high, it might explode. If too low, it might not learn

def train(category_tensor, line_tensor): # train per surgeme
    hidden = rnn.initHidden() # take entire sequence

    rnn.zero_grad()
    # print(line_tensor.size()[0])
    # print("line_tensor.size()[0]")
    for i in range(line_tensor.size()[0]):
        # rnn.zero_grad()
        # output, hidden = rnn(line_tensor[i], hidden)
        output, hidden = rnn(line_tensor[i], hidden)

        # per batch update and calcualte loss
        output = output.float()
        category = category_tensor[i][0]
        # print("category")
        # print(category)
        loss = criterion(output, category)
        loss.backward(retain_graph=True)

        # Add parameters' gradients to their values, multiplied by learning rate
        # print(type(rnn.parameters()))
        # print((rnn.parameters()).size())
        for p in rnn.parameters():
            # print(type(p))
            # print((p))
            if p.grad is not None: # very important line
                p.data.add_(-learning_rate, p.grad.data)

        # p.grad -= learning_rate * p.grad
    # sys.exit()
    return output, loss.item()


import time
import math

# n_iters = 100000
print_every = 500
plot_every = 50000

# Keep track of losses for plotting
current_loss = 0
all_losses = []

def timeSince(since):
    now = time.time()
    s = now - since
    m = math.floor(s / 60)
    s -= m * 60
    return '%dm %ds' % (m, s)

start = time.time()

xdata = []
ydata = []
count_train = 0
current_surgeme = y[0]
prev_surgeme = y[0]
accum = []
accum_y = []
batch_size = 1
for i in range(0, (x.size())[0]):
    current_surgeme = y[i]
#     print (current_surgeme, prev_surgeme)
    if ((i+1) % batch_size) == 0:
        xdata.append(accum)
        ydata.append(accum_y)
        accum = []
        accum_y = []
    # accum = []
    # accum.append(x[i])
    # accum.append(x[i])
    # xdata.append(accum)
    # ydata.append(y[i])
    # prev_surgeme = current_surgeme
    accum.append(x[i])
    accum_y.append(y[i])

totalsurgeme = len(xdata)
traincount = int(totalsurgeme * 0.8)
testcount = int(totalsurgeme - traincount)


xtrain = xdata[0:traincount:1]
ytrain = ydata[0:traincount:1]

xtest = xdata[traincount:traincount+testcount:1]
ytest = ydata[traincount:traincount+testcount:1]


def listToTensor(llist): # line -> surgeme
    tensor = torch.zeros(len(llist), 1, len(llist[0]), dtype=torch.float64)
    for li, letter in enumerate(llist):
#         print ("letterli")
#         print (li)
#         print (letter)
#         tensor[li][0][letterToIndex(letter)] = 1
        tensor[li] = letter
    return tensor
def listToTensorInt(llist): # line -> surgeme
    tensor = torch.zeros(len(llist), 1, 1, dtype=torch.long)
    for li, letter in enumerate(llist):
#         print ("letterli")
#         print (li)
#         print (letter)
#         tensor[li][0][letterToIndex(letter)] = 1
        tensor[li] = letter
    return tensor
nepoch = 20

for epoch in range(0, nepoch):
    current_loss = 0
    n_iters = len(xtrain)
    # prev_gt_surgeme = ytrain[0].item()
    print ("hidden = rnn.initHidden()")
    # hidden = rnn.initHidden()
    for iter in range(0, n_iters):
    #     cateory, line, category_tensor, line_tensor = randomTrainingExample()

        # category_tensor = torch.tensor([ytrain[iter]], dtype=torch.long)
        category_tensor = listToTensorInt(ytrain[iter])
        line_tensor = listToTensor(xtrain[iter])

        output, loss = train(category_tensor, line_tensor)
        current_loss += loss


        # category = category_tensor.item()
        # prev_gt_surgeme = category # update prev surgme
        # # print iter number, loss, name, and guess
        if iter % print_every == 0:
            # guess, guess_i = categoryFromOutput(output)
            # correct = '✓' if guess == category else '✗ (%s)' % category
    #         print('%d %d%% (%s) %.4f %s / %s %s' % (iter, iter / n_iters * 100, timeSince(start), loss, line, guess, correct))
            print ("Epoch= "+str(epoch)+" Loss = "+ str(loss))
        # Add current loss avg to list of losses
        # if iter % plot_every == 0:
        #     all_losses.append(current_loss / plot_every)
        #     current_loss = 0
#     import matplotlib.pyplot as plt
#     import matplotlib.ticker as ticker

#     plt.figure()
#     plt.plot(all_losses)


# In[ ]:

# import matplotlib.pyplot as plt
# import matplotlib.ticker as ticker

# plt.figure()
# plt.plot(all_losses)


# In[ ]:

# Keep track of correct guesses in a confusion matrix
print ("Training accuracy")
correct = 0
incorrect = 0
confusion = torch.zeros(n_categories, n_categories)


n_confusion = len(xtrain)

# Just return an output given a line
def evaluate_train(line_tensor):
    hidden = rnn.initHidden()

    for i in range(line_tensor.size()[0]):
        output, hidden = rnn(line_tensor[i], hidden)

    return output

# Go through a bunch of examples and record which are correctly guessed
for i in range(len(xtrain)):
#     category, line, category_tensor, line_tensor = randomTrainingExample()

    category_tensor = torch.tensor([ytrain[i]], dtype=torch.long)
    line_tensor = listToTensor(xtrain[i])
    category = category_tensor.item()

    output = evaluate_train(line_tensor)
    guess, guess_i = categoryFromOutput(output)
#     print (guess, guess_i)
    category_i = all_categories.index(category)
    if guess_i == category_i:
        correct += 1
    else:
        incorrect += 1

    # if guess_i > 7 or category_i > 7:
    #     print ("Why 7?")
#     if
    confusion[category_i][guess_i] += 1

# Normalize by dividing every row by its sum
for i in range(n_categories):
    confusion[i] = confusion[i] / confusion[i].sum()

acc = (correct * 1.0)/ (correct + incorrect)
print ("Correct = "+str(correct))
print ("InCorrect = "+str(incorrect))
print ("Accuracy = "+str(acc))
train_accuracy = acc * 100.0

# In[ ]:

# Keep track of correct guesses in a confusion matrix
print ("Testing accuracy")
# xtest = xtest_list
correct = 0
incorrect = 0
confusion = torch.zeros(n_categories, n_categories)

n_confusion = len(xtest)




# Just return an output given a line
def evaluate(line_tensor):
    hidden = rnn.initHidden() # need to check

    for i in range(line_tensor.size()[0]):
        output, hidden = rnn(line_tensor[i], hidden)

    return output
fw_test = open("perframe_prediction_taurus_pegpole.txt", "w")
# Go through a bunch of examples and record which are correctly guessed

# initialize
category_tensor = torch.tensor([ytest[0]], dtype=torch.long)
category = category_tensor.item()
prev_surgeme = category

for i in range(len(xtest)):
#     category, line, category_tensor, line_tensor = randomTrainingExample()

    category_tensor = torch.tensor([ytest[i]], dtype=torch.long)
    line_tensor = listToTensor(xtest[i])
    category = category_tensor.item()

    output = evaluate(line_tensor)
    if constraint_filter == True: # add constraint before prediction
        constraint = constraints[prev_surgeme]
        if len(constraint) > 0:
            # print("Applying constraint filter")
            # print ("prev_pred_surgeme")
            # print (prev_surgeme)
            # print(output)
            # print(category_tensor)
            prob = torch.max(output)
            # print (prob)
            prob_val = prob.item() # make sure it's minimum value
            # print("minprob_val")
            # print(prob_val)
            # print(output.size()[1])
            con_filter = output
            # print("con_filter before")
            # print(con_filter)
            # for j in range(con_filter.size()[1]): # initialize with max value of the output tensor
            #     con_filter[0][j] = (-1) * (maxprob) #  negate for substraction
            # # constraint = constraints[prev_pred_surgeme]
            # print("con_filter after")
            # print(con_filter)
            # if len(constraint) > 0: # if has constaints
            for j in range(con_filter.size()[1]):
                sindex = j + 1
                if sindex in constraint: # constrainted surgeme value changed to current_value+highest prob. to increase likelyhood
                    con_filter[0][sindex] = 1.0 * prob_val + con_filter[0][sindex]
            # print("con_filter after zeroing out")
            # print(con_filter)
            output = con_filter # nullify non-constaints surgeme
            # print("After filter output")
            # print(output)
    guess, guess_i = categoryFromOutput(output)
#     print (guess, guess_i)


    category_i = all_categories.index(category)
    fw_test.write(str(category_i)+"\t" + str(guess_i) + "\n")
    if guess_i == category_i:
        correct += 1
    else:
        incorrect += 1
    prev_surgeme = guess_i # update prev surgeme
    # if guess_i > 7 or category_i > 7:
    #     print ("Why 7?")
#     if
    confusion[category_i][guess_i] += 1

# Normalize by dividing every row by its sum
for i in range(n_categories):
    confusion[i] = confusion[i] / confusion[i].sum()

acc = (correct * 1.0)/ (correct + incorrect)
print ("Correct = "+str(correct))
print ("InCorrect = "+str(incorrect))
print ("Accuracy = "+str(acc))
test_accuracy = acc * 100.0

fw.write(str(nepoch) + "\t" + str(train_accuracy) + "\t" + str(test_accuracy) + "\t" + str(traincount)+ "\t" + str(testcount) +"\n")

fw.close()
fw_test.close()


