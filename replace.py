#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
创 建 者: wulangzhou
"""

class Node(object):
    """构造一个类用来保存树的节点信息
    """
    def __init__(self):
        """
        构造函数初始化
        """
        self.old_word = ''
        self.new_word = ''
        self.next_node = {}
        self.is_end = False


class Replacer(object):
    """ pass
    """
    def __init__(self):
        """
        构造函数初始化
        """
        self.root = Node()

    def insert_list(self, reocrds, force=True):
        """ 多个需要替换的字符,以链表的方式插入
        """
        for old_word, new_word in reocrds:
            self.root.insert(old_word, new_word, force=force)

    def insert_dict(self, dic, force=True):
        """ 多个需要替换的字符,以字典的方式插入
        """
        for key, val in dic.items():
            self.root.insert(key, val, force=force)

    def insert(self, old_word, new_word, force=True):
        """ 从根节点插入一个需要替换的词
        """
        if not isinstance(old_word, basestring):
            raise Exception('第2个需要替换的参数必须为字符串')
        if not isinstance(new_word, basestring):
            raise Exception('第3个替换成需要的参数必须为字符串')
        if isinstance(old_word, str):
            old_word = old_word.decode('utf-8')
        if isinstance(new_word, str):
            new_word = new_word.decode('utf-8')

        p = self.root
        for word in old_word:
            ord_num = ord(word)
            if not p.next_node.get(ord_num):
                p.next_node[ord_num] = Node()
            p = p.next_node[ord_num]
        if force:
            p.old_word = old_word
            p.new_word = new_word
        p.is_end = True

    def replace(self, paragraph):
        """ 将paragraph 里面的字符替换成想要的字符
        """
        if not isinstance(self.root, Node):
            raise Exception('第1个参数必须为Node类型的根节点')
        if not isinstance(paragraph, basestring):
            raise Exception('第2个参数必须为一个需要替换的字符串')
        if isinstance(paragraph, str):
            paragraph = paragraph.decode('utf-8')

        old_paragraph, new_paragraph = [word for word in paragraph], ''

        index, len_paragraph = 0, len(old_paragraph)
        while index < len_paragraph:
            p, find_word = self.root, None
            for j in xrange(index, len_paragraph):
                ord_num = ord(old_paragraph[j])
                if not p.next_node.get(ord_num):
                    break
                p = p.next_node[ord_num]
                if p.is_end:
                    find_word = p
            if find_word:
                new_paragraph = '%s%s' % (new_paragraph, find_word.new_word)
                index += len(find_word.old_word)
            else:
                new_paragraph = '%s%s' % (new_paragraph, old_paragraph[index])
                index += 1
        return new_paragraph


if __name__ == '__main__':
    replacer = Replacer()
    replacer.insert('吴浪舟', 'wulangzhou')
    replacer.insert('吴浪舟', 'wulangzhou')
    replacer.insert('闫伟鸿', '研伟鸿1989')
    replacer.insert('闫伟鸿', '研伟鸿1989')
    replacer.insert('#039', '')
    old_str = '那个吴浪舟在做什么，#039闫伟鸿啊知道'
    new_str = replacer.replace(old_str)
    print "old_str=", old_str
    print "new_str=", new_str
