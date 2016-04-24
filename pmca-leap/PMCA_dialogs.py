#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
sys.argv = [''] 
import os

import time

#インポートパスにカレントディレクトリを加える
sys.path.append(os.getcwd())

import PMCA
import PyPMCA

from tkinter import *
from tkinter.ttk import *


class QUIT:
	def __init__(self, root):
		self.root = root
	
	def __call__(self):
		self.root.winfo_toplevel().destroy()
		self.root.quit()

class SCALE_DIALOG_FANC:
	def __init__(self, app, root, sel):
		self.app =app
		self.root=root
		self.sel =sel
		self.root.title(self.app.transform_list[self.sel].name)
		self.var =None
		self.tvar =StringVar()
		self.refbone = []
		self.refbone_index=[]
		
		self.app.transform_data.append(PyPMCA.MODEL_TRANS_DATA(name = self.app.transform_list[self.sel].name ,scale=1.0, bones=[], pos=[0.0, 0.0, 0.0], rot=[0.0, 0.0, 0.0], props={}))
		self.data = self.app.transform_data[-1]
		for i,x in enumerate(self.app.transform_list[self.sel].bones):
			self.data.bones.append(PyPMCA.BONE_TRANS_DATA(name=x.name))
	
	def CANCEL(self):
		self.app.transform_data.remove(self.data)
		self.app.refresh(level=2)
		self.root.winfo_toplevel().destroy()
		self.root.quit()
	
	def OK(self):
		self.app.transform_data[0].scale = self.data.scale * self.app.transform_data[0].scale
		for i,x in enumerate(self.app.transform_data[0].pos):
			x += self.data.pos[i]
		for i,x in enumerate(self.app.transform_data[0].rot):
			x += self.data.rot[i]
		for x in self.data.bones:
			tmp = None
			for y in self.app.transform_data[0].bones:
				if y.name==x.name:
					tmp = y
					break
			else:
				self.app.transform_data[0].bones.append(PyPMCA.BONE_TRANS_DATA(name=x.name))
				tmp = self.app.transform_data[0].bones[-1]
			
			tmp.length = tmp.length*x.length
			tmp.thick = tmp.thick * x.thick
			for i,y in enumerate(tmp.pos):
				y += x.pos[i]
			for i,y in enumerate(tmp.rot):
				y += x.rot[i]
		self.app.transform_data.remove(self.data)
		self.app.refresh(level=2)
		for x in self.app.transform_data[0].bones:
			print(x.name)
		
		self.root.winfo_toplevel().destroy()
		self.root.quit()
		
		
	def change_scale(self, event):
		var = self.var.get()
		self.tvar.set('%.3f'%var)
		
		self.change_var(var)
	
	def change_spinbox(self):
		var = float(self.tvar.get())
		self.var.set(var)
		self.change_var(var)
	
	def enter_spinbox(self, event):
		try:
			var = float(self.tvar.get())
		except:
			var = self.var.get()
			return None
		
		self.tvar.set('%.3f'%var)
		self.var.set(var)
		self.change_var(var)
	
	def change_var(self, var):
		weight = self.app.transform_list[self.sel].scale
		self.data.scale = weight * var+1-weight
		
		weight = self.app.transform_list[self.sel].pos
		for i,x in enumerate(weight):
			self.data.pos[i] = x * var
		
		weight = self.app.transform_list[self.sel].rot
		for i,x in enumerate(weight):
			self.data.rot[i] = x * var
		'''
		print(var)
		print(self.app.transform_list[self.sel].pos)
		print(self.data.pos)
		'''
		
		#print(self.app.transform_list[self.sel].name, len(self.app.transform_list[self.sel].bones))
		for i,x in enumerate(self.app.transform_list[self.sel].bones):
			self.data.bones[i].length = x.length * var+1-x.length
			self.data.bones[i].thick = x.thick * var+1-x.thick
			for j,y in enumerate(x.pos):
				self.data.bones[i].pos[j] = y * var
			for j,y in enumerate(x.rot):
				self.data.bones[i].rot[j] = y * var
		self.app.refresh(level=2)
		
class SETTING_DIALOG_FANC:
	def __init__(self, app, root):
		self.app =app
		self.root=root
		if PyPMCA.lang == 'en':
		   self.root.title("PMCA global settings")
		else:
		   self.root.title("PMCA設定")
		
		self.flag_export2folder = BooleanVar()
		self.flag_export2folder.set(app.settings.export2folder)
		
	
	def apply_all(self):
		self.app.settings.export2folder = self.flag_export2folder.get()
	
	def OK(self):
		self.root.winfo_toplevel().destroy()
		self.root.quit()
		

