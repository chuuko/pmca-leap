# -*- coding: utf-8 -*-

'''
for PMCAv0.0.6 beta2
重複するボーン名を変更して同名のボーンがあってもマージされないようにします
引数：なし

'''

import re

print('プレスクリプト')
print('ボーンマージ無効')
model = Get_PMD(0)
model2 = Get_PMD(4)
maxnum = -1
for y in model.bone:
	y.tmpname = None
	y.tmpnum = -1

for x in model2.bone:
	print(x.name)
	name = x.name.replace('+','\+')
	name = name.replace('*','\*')
	name = name.replace('.','\.')
	name = name.replace('(','\(')
	name = name.replace(')','\)')
	name = name.replace(']','\]')
	name = name.replace('[','\[')
	p = re.compile('%s-?\d*\Z'%(name))
	for y in model.bone:
		#print('##',y.name)
		m = p.match(y.name)
		if m != None:
			#print('一致')
			y.tmpname = x.name
			p2 = re.compile('-\d+\Z')
			m2 = p2.search(m.group())
			if m2 != None:
				#番号付き一致
				#print(m2.group())
				y.tmpnum = int(m2.group()[1:])
				if maxnum < y.tmpnum:
					maxnum = y.tmpnum
			else:
				#番号なし一致
				#print('番号なし一致',m.group(),y.tmpname)
				y.tmpnum = 0
				if maxnum < 0:
					maxnum = 0
	#print(x.name)

maxnum += 1

if maxnum > 0:	
	for x in model2.bone:
		#print(x.name)
		x.name = '%s-%d'%(x.name,maxnum)

	for x in model.bone:
		#print(x.name, x.tmpname, x.tmpnum)
		if x.tmpnum == 0:
			x.name = '%s-%d'%(x.tmpname,x.tmpnum)

	Set_PMD(0, model)
	Set_PMD(4, model2)

'''
print('######bones')
for x in model.bone:
	print(x.name)
print('######bones2')
for x in model2.bone:
	print(x.name)
'''

