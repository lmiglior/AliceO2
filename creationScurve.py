#!/usr/bin/env python
import matplotlib.pyplot as plt
import pandas as pd
from pandas import Series, DataFrame
import numpy as np
import glob
from scipy import special
from scipy.optimize import curve_fit
import time
def erfunc(x,a,b,c,d):
    return a*special.erf((x-b)/c)+d

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
#vpulse=pd.Series()
#hits=pd.Series()


for ch in range(len(chipnumber)):
     df_collection[ch]=df[(df["chip"]==ch)]
     df_collection[ch].reset_index(inplace=True)
     #print(df_collection[0])
     for col in range(0,1024):
         #vpulse=df_collection[ch].loc[col,'vpulsel']
         #print(vpulse)
         df_new_collection=df_collection[ch][(df_collection[ch]["col"]==col)]
         hits=df_new_collection["trig"]
         vpulse=df_new_collection["vpulsel"]
         #print(df_new_collection)
         popt, pcov = curve_fit(erfunc,vpulse,hits, maxfev=100000,p0=[25,160,100,26])
#         print(erfunc(vpulse,*popt))
         print(time.clock())
         #plt.scatter(vpulse,hits,c="b",marker=r'$\clubsuit$')
         #plt.plot(vpulse,erfunc(vpulse,*popt))
         break
      #.diff().fillna(0))
     #print(vpulse)
#        print(vpul        #plt.plot(vpulse, hits, c="g", alpha=0.5, marker=r'$\clubsuit$')
    #plt.plot(hits,special.erf(hits))
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
