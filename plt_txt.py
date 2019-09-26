import numpy as np
import matplotlib.pyplot as plt
import glob
import os

#path = "/home/dhaunss/workdir/datadir/txtfiles/noisy/*.txt"
#files = glob.glob(path)

path = "/home/dhaunss/workdir/datadir/txtfiles/*/*.txt"
#outpath="/home/dhaunss/workdir/datadir/txtfiles/denoised/plots/"
files=glob.glob(path)
print(files)
for name in files:
#		file_name = os.path.basename(name)
#		identifier = os.path.splitext(file_name)[0]
		data = open(name, "r")
		print("loading:",name)
		lines= data.readlines()
		tmp = [line[:-1] for line in lines]
		array = []
		for i in tmp:
			array.append(float(i))
		plt.plot(array)	
		plt.savefig(f"{name}.png")
		plt.close()




#<module> RdChannelNoiseImporter               </module>     <!-- add this module if you want to add noise              -->
