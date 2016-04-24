#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import subprocess

import time

#インポートパスにカレントディレクトリを加える
sys.path.append(os.getcwd())
def main():
	argvs = sys.argv  # コマンドライン引数を格納したリストの取得
	
	tmp = argvs[0].rsplit('/', 1)
	tmp = tmp.rsplit('/', 1)
	print(tmp)
	
	curd = tmp[0]
	
	path = '%s/PMCA_1.0-2.0converter'%(curd)
	
	v1_v2(path, argvs[1:])

def v1_v2(path, argvs):
	print(argvs)
	
	for x in argvs:
		cmd = '%s %s'%(path,x)
		print(cmd)
			
		retcode = subprocess.call(cmd, shell=True)
		
		fp = open('%s'%(x), 'r', encoding = 'cp932')
		lines = fp.read()
		fp.close()
		
		fp = open('%s'%(x), 'w', encoding = 'utf-8')
		fp.write(lines)
		fp.close()
		print('文字コードをUTF8に変換しました')
		

if __name__ == "__main__":
	main()
