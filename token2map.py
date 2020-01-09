#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'wulangzhou'

import os
import sys
import time
import ctypes
import datetime
from ctypes import c_char_p, c_int, c_float, c_double, c_bool, c_void_p, py_object

LID = ctypes.CDLL('%s/token2map_lib.so' % '.')

LID.get_root_prx.argtypes = []
LID.get_root_prx.restype = c_void_p

LID.free_root_prx.argtypes = [c_void_p]

LID.insert_prx.argtypes = [c_void_p, c_char_p, py_object]
LID.insert_prx.restype = c_bool

LID.remove_prx.argtypes = [c_void_p, c_char_p]
LID.remove_prx.restype = c_bool

LID.cutall_prx.argtypes = [c_void_p, c_char_p]
LID.cutall_prx.restype = py_object

LID.getall_prx.argtypes = [c_void_p, c_char_p]
LID.getall_prx.restype = py_object

LID.get_phrase_prefix_infos_prx.argtypes = [c_void_p, c_char_p]
LID.get_phrase_prefix_infos_prx.restype = py_object

LID.get_phrase_suffix_infos_prx.argtypes = [c_void_p, c_char_p]
LID.get_phrase_suffix_infos_prx.restype = py_object

LID.get_lcp_suffix_infos_prx.argtypes = [c_void_p, c_char_p]
LID.get_lcp_suffix_infos_prx.restype = py_object

LID.exist_phrase_prx.argtypes = [c_void_p, c_char_p]
LID.exist_phrase_prx.restype = c_bool

LID.get_kv_prx.argtypes = [c_void_p, c_char_p]
LID.get_kv_prx.restype = py_object 

LID.cutmax_prx.argtypes = [c_void_p, c_char_p]
LID.cutmax_prx.restype = py_object

LID.test.restype = py_object


class Tree(object):
    """ pass
    """
    def __init__(self):
        """ pass
        """
        self.root = LID.get_root_prx()

    def insert(self, content, oinfo):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.insert_prx(self.root, content, oinfo)

    def remove(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.remove_prx(self.root, content)

    def exist(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.exist_phrase_prx(self.root, content)

    def get_kv(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.get_kv_prx(self.root, content)

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
        return LID.get_phrase_prefix_infos_prx(self.root, content)

    def get_suffix(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.get_phrase_suffix_infos_prx(self.root, content)

    def get_lcp_suffix(self, content):
        """ pass
        """
        if not isinstance(content, str):
            content = content.encode('utf8')
        return LID.get_lcp_suffix_infos_prx(self.root, content)

    def free_root(self):
        """ pass
        """
        LID.free_root_prx(self.root)

    def test(self):
        """ pass
        """
        return LID.test()


if __name__ == '__main__':
    idx = 0;
    while True:
        root = Tree()
        #print 'insert 2',
        root.insert('2', 1)
        root.test()

        #root.remove('2')
        #print root.get_kv('2')

        #print 'insert 1',
        root.insert('1', (1,2))
        #print 'insert 9',
        root.insert('9', 3)
        #print 'insert 12',
        root.insert('9', 4)
        #print 'insert 23',
        root.insert('23', 5)
        root.insert('23', 5)
        root.insert('23', 5)
        root.insert('23', 5)
        root.insert('23', 5)
        root.insert('23', 5)
        root.insert('23', 5)
        root.insert('23', 5)
        root.insert('23', {2: 2})
        #print 'insert 24',
        root.insert('24', 5)
        #print 'insert 25',
        root.insert('25', 6)
        #print 'insert 14',
        root.insert('14', (3, 4))
        #print 'insert 123',
        root.insert('123', {3: 0})
        #print 'insert 124',
        root.insert('124', 9)
        #print 'insert 吴浪舟',
        root.insert('吴浪舟', 10)
        #print 'insert 吴浪',
        root.insert('吴浪', 11)
        root.insert('浪舟', 12)
        root.insert('舟', 12)
        root.insert('浪', 13)
        print 'end insert'
        print len(root.get_all('吴浪' * 11000))
        import pdb
        pdb.set_trace()

        print root.remove('222'), '++'
        print root.remove('浪'), '++'
        print root.remove('124'), '++'
        print root.exist('吴浪'), '----'
        print root.remove('吴浪'), '++'
        print root.exist('吴浪'), '----'
        print 'end remove'

        #print root.get_kv('23'), '===='

        print root.remove('1'), '++'
        print root.get_kv('1'), '===='

        print root.get_kv('111'), '===='
        print 'end get_kv'

        for i in xrange(1, 20):
            phrse_position_list = root.get_all('1411234')
            for phrase, position, oinfo in phrse_position_list:
                print 'cut_all, 1411234', phrase, position, oinfo
        print 'end get_all'

        for i in xrange(1, 20):
            phrse_position_list = root.cut_all('1411234')
            for phrase, position, oinfo in phrse_position_list:
                print 'cut_all, 1411234', phrase, position, oinfo
        print 'end cut_all'

        for i in xrange(1, 20):
            phrse_position_list = root.cut_max('1411234')
            for phrase, position, oinfo in phrse_position_list:
                print 'cut_all, 1411234', phrase, position, oinfo
        print 'end cut_max'

        for i in xrange(1, 20):
            phrse_position_list = root.get_prefix('123456')
            for phrase in phrse_position_list:
                print 'prefix 123456', phrase
        print 'end prefix'

        for i in xrange(1, 20):
            phrse_position_list = root.get_suffix('吴')
            for phrase in phrse_position_list:
                print 'sufix 吴', phrase
        print 'end suffix'

        print 'exist 123', root.exist('123')
        print 'exist 125', root.exist('125')
        print 'exist 1', root.exist('1')
        print 'end exist'

        idx += 1
        print idx
        root.free_root()
