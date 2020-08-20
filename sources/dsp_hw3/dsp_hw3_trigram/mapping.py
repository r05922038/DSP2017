
# coding: utf-8

# In[6]:

import sys


# In[80]:

f1=sys.argv[1]
f2=sys.argv[2]


# In[7]:

#f1="Big5-ZhuYin.map"
#f2="test.map"


# In[8]:

zy_big5=dict()
with open(f1,'r') as f:
    for line in f:
        line= line.split()
        big5=line[0]
        tokens=line[1].decode('big5').split('/')
        zys=set([x[0].encode('big5') for x in tokens])
        for zy in zys:
            if zy in zy_big5:
                zy_big5[zy].add(big5)
            else:
                zy_big5[zy]=set([big5])


# In[9]:

big5set=set([])
with open(f2,'w') as f:
    for zy in zy_big5:
        f.write(zy+' '+' '.join(zy_big5[zy])+'\n')
        for big5 in zy_big5[zy]:
            if big5 not in big5set:
                f.write(big5+' '+big5+'\n')
                big5set.add(big5)


# In[ ]:



