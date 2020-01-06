#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'wulangzhou'

import os
import sys
import time
import ctypes
import datetime
from ctypes import c_char_p, c_int, c_float, c_double, c_bool, c_void_p, py_object

LID = ctypes.CDLL('%s/token_lib.so' % '.')

LID.get_root_prx.argtypes = []
LID.get_root_prx.restype = c_void_p

LID.free_root_prx.argtypes = [c_void_p]

LID.insert_prx.argtypes = [c_void_p, c_char_p]
LID.insert_prx.restype = c_bool

LID.remove_prx.argtypes = [c_void_p, c_char_p]
LID.remove_prx.restype = c_bool

LID.cutall_prx.argtypes = [c_void_p, c_char_p]
LID.cutall_prx.restype = py_object

LID.getall_prx.argtypes = [c_void_p, c_char_p]
LID.getall_prx.restype = py_object

LID.cutmax_prx.argtypes = [c_void_p, c_char_p]
LID.cutmax_prx.restype = py_object

LID.get_prefix_phrases_prx.argtypes = [c_void_p, c_char_p]
LID.get_prefix_phrases_prx.restype = py_object

LID.get_suffix_phrases_prx.argtypes = [c_void_p, c_char_p]
LID.get_suffix_phrases_prx.restype = py_object

LID.exist_phrase_prx.argtypes = [c_void_p, c_char_p]
LID.exist_phrase_prx.restype = c_bool


class Tree(object):
    """ pass
    """
    def __init__(self):
        """ pass
        """
        self.root = LID.get_root_prx()

    def insert(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.insert_prx(self.root, content)

    def cut_all(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.cutall_prx(self.root, content)

    def cut_max(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.cutmax_prx(self.root, content)

    def get_all(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.getall_prx(self.root, content)

    def get_prefix(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.get_prefix_phrases_prx(self.root, content)

    def get_suffix(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.get_suffix_phrases_prx(self.root, content)

    def is_exist(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.exist_phrase_prx(self.root, content)

    def free_root(self):
        """ pass
        """
        LID.free_root_prx(self.root)


if __name__ == '__main__':
    idx = 0;
    while True:
        root = Tree()
        #print 'insert 2',
        root.insert('2')
        #print 'insert 1',
        root.insert('1')
        #print 'insert 9',
        root.insert('9')
        #print 'insert 12',
        root.insert('9')
        #print 'insert 23',
        root.insert('23')
        #print 'insert 24',
        root.insert('24')
        #print 'insert 25',
        root.insert('25')
        #print 'insert 14',
        root.insert('14')
        #print 'insert 123',
        root.insert('123')
        #print 'insert 124',
        root.insert('124')
        #print 'insert 吴浪舟',
        root.insert('吴浪舟')
        #print 'insert 吴浪',
        root.insert('吴浪')
        root.insert('浪舟')
        root.insert('舟')
        root.insert('浪')

        phrse_position_list = root.cut_max('1吴浪2舟舟浪吴浪2')
        for phrase, position in phrse_position_list:
            print 'cut_all, 12323242', phrase, position
        print ''

        """
        phrse_position_list = root.get_prefix('123456')
        for phrase in phrse_position_list:
            print 'prefix 123456', phrase
        print ''

        phrse_position_list = root.get_suffix('吴')
        for phrase in phrse_position_list:
            print 'sufix 吴', phrase
        print ''

        print 'exist 123', root.is_exist('123')
        print 'exist 125', root.is_exist('125')
        print 'exist 1', root.is_exist('1')
        """
        root.free_root()
        print 'idx=%s' % idx 
        idx += 1
        break
