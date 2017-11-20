#!/usr/bin/python
# coding=utf-8

import json

class parse_cmd():
    def __init__(self,cmd_define_path):
        if cmd_define_path == None or cmd_define_path == '' :
            print 'cmd_define_path empty'
            return
        else:
            print cmd_define_path

        #读取命令配置文件
        try:
            cmd_file = open(cmd_define_path)
            content = cmd_file.read()
            self.cmd_def_list = json.loads(content) 
        except:
            print 'read commands definition file failed'


    def parse(self,text):
        '''
            从输入的一段文本中解析出对应的命令，成功返回命令id，否则返回-1
        '''
        if text == None or text == '' :
            return -1

        for cmd_dict in self.cmd_def_list :
            pass
            
        return -1