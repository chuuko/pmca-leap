# -*- coding: utf-8 -*-

'''
for PMCAv0.0.6 r6
指定したボーンを全ての親にします
引数：全親ボーン名
'''

model = Get_PMD(0)
index = 65535
for i,x in enumerate(model.bone):
	if x.name == argv[-1]:
		index = i
		break

for i,x in enumerate(model.bone):
	if x.parent == 65535:
		if x.name != argv[-1]:
			model.bone[i].parent = index
'''
for x in model.IK_list:
	print(x.weight)
	
while True:
	pass
'''
Set_PMD(0, model)

