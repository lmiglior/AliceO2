#!/usr/bin/env python
import matplotlib.pyplot as plt
import pandas as pd
from pandas import Series, DataFrame
import numpy as np
import glob
from scipy import special

AllFiles=np.sort(glob.glob("digit_coo*.txt"))
df=pd.DataFrame()
print("How many files ?",len(AllFiles))
df_collection={}

for file in AllFiles:
    #    for ch in range(len(chipnumber)):
    #    while ch < len(chipnumber):
    df1=pd.read_csv(file,sep=" ", header=None)
    lendf=len(df1.index)
    df3=pd.DataFrame(np.full(lendf,file))
    df2=pd.concat([df3,df1],axis=1)
    df=df.append(df2)
    print(file)
df.columns=["file","chip","col","row","trig","vpulsel"]
chipnumber=set(df["chip"])

print("How many chip?",len(chipnumber))

df_new_collection={}

for ch in range(0,1):
     df_collection[ch]=df[(df["chip"]==0)]
     df_collection[ch].reset_index(inplace=True)
     for col in range(0,1024):
        df_new_collection=df_collection[ch][(df_collection[ch]["col"]==col)]
        #x=df_new_collection.sub(["vpulsel"],axis='columns')
        hits=df_new_collection["trig"]
        #.diff().fillna(0)
        vpulse=(df_new_collection["vpulsel"])
        #.diff().fillna(0))
        #z=(y/x).dropna()
        #print(vpulse)
#        plt.scatter(hits, vpulse, c="g", alpha=0.5, marker=r'$\clubsuit$')
        #print(z)
plt.show()
        

# A=set(df["chip"])
# print("Chips' names?",A)
# print(len(A))
# # for chip in range(0,len(A)):
# #     print(chip)
# #     for col in range(0,1024):
# df_new=pd.DataFrame()
# for col in range(0,1024):
#     df_new1=df[(df["col"]==col)&(df["chip"]==0)]
#     df_new=df_new.append(df_new1)

# print(df_new)
