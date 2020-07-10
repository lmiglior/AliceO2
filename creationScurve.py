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

AllFiles=np.sort(glob.glob("Thr/digit_coo*.txt"))
df=pd.DataFrame()
print("How many files ?",len(AllFiles))
df_collection={}

for file in AllFiles:
    df1=pd.read_csv(file,sep=" ", header=None)
    lendf=len(df1.index)
    df3=pd.DataFrame(np.full(lendf,file))
    df2=pd.concat([df3,df1],axis=1)
    df=df.append(df2)
    print(file)
df.columns=["file","chip","col","row","trig","vpulsel"]
chipnumber=set(df["chip"])

print("How many chips?",len(chipnumber))

df_new_collection={}
noise=[]
thr=[]
rmsthr=[]
for ch in range(len(chipnumber)):
     df_collection[ch]=df[(df["chip"]==ch)]
     df_collection[ch].reset_index(inplace=True)
     for col in range(0,1024):
         df_new_collection=df_collection[ch][(df_collection[ch]["col"]==col)]
         hits=df_new_collection["trig"]
         vpulse=df_new_collection["vpulsel"]
         diff=hits.diff()/vpulse.diff()
         noise=(diff.fillna(0)).max()
         popt, pcov = curve_fit(erfunc,vpulse,hits, maxfev=10000000,p0=[25,160,100,26])
         thr=popt[1]
         print(popt)
         rmsthr=popt[2]
         print("Finish analysis chip ",ch," duration ",time.clock())
         break

plt.show()
        
