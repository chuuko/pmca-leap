# -*- coding: utf-8 -*-

'''
for PMCAv0.0.6 beta2
指定したボーンをパーツの親にします
引数：親ボーン名
'''

parent = BONE(argv[-1], argv[-1], 65535, 0, 1, 0, [0.0,0.0,0.0])

model = Get_PMD(4)
for x in model.bone:
	if x.parent == 65535:
		x.parent = len(model.bone)

model.bone.append(parent)

Set_PMD(4, model)

