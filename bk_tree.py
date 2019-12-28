# !/usr/bin/env python
# -*- coding: utf-8 -*-

""" @author:wlz
"""

import Queue
import Levenshtein


class Node(object):
    """ 节点
    """

    def __init__(self, string=None, is_valid=True, oinfo=None):
        """ 构造一个节点
        :type is_valid: 用于删除字符的标记
        :type string: 插入字符
        """
        self.string = string
        self.next_node_dict = {}
        self.is_valid = is_valid
        self.oinfo = oinfo

    def insert(self, phrase, oinfo=None):
        """ 插入字词
            :param phrase: 插入的字符
        """
        if isinstance(phrase, str):
            phrase = phrase.decode('utf-8')

        if self.string is None:
            self.string = phrase
            return

        p = self
        while p is not None:
            distance = Levenshtein.distance(p.string, phrase)
            next_node = p.next_node_dict.get(distance)
            if next_node is None:
                p.next_node_dict[distance] = Node(string=phrase, is_valid=True, oinfo=oinfo)
            p = next_node

    def find(self, phrase, fix_distance):
        """ 查找编辑距离在 distance 以内的词
            :param phrase: 被查的字符串
            :param fix_distance: 在多少范围以内的
        """
        if isinstance(phrase, str):
            phrase = phrase.decode('utf-8')

        if self.string is None:
            return []

        res, next_nodes = [], Queue.Queue()
        next_nodes.put(self)
        while not next_nodes.empty():
            node = next_nodes.get()
            distance = Levenshtein.distance(node.string, phrase)
            if distance <= fix_distance and node.is_valid:
                res.append((node, distance))
            min_dis = distance - fix_distance
            max_dis = distance + fix_distance
            for dis in range(min_dis, max_dis + 1):
                next_node = node.next_node_dict.get(dis)
                if next_node:
                    next_nodes.put(next_node)
        return res

    def delete(self, phrase):
        """ 删除一个字符串，从树里面
            :param phrase: 被删除的字符串
        """
        nodes = self.find(phrase, 0)
        if nodes:
            nodes[0].is_valid = False

    @staticmethod
    def copy_tree(new_root, now_node):
        """ 把一颗旧树复制到一个新树
            :param new_root: 新树
            :param now_node: 旧树
        """
        if now_node.is_valid:
            new_root.insert(now_node.string,
                            is_valid=now_node.is_valid,
                            oinfo=now_node.oinfo)
        for node in now_node.next_node_dict.values():
            Node.copy_tree(new_root, node)

    @staticmethod
    def new_tree(root=None):
        """ 新建一颗树
            :param root: 生成一个节点
        """
        new_root = Node()
        if root is None:
            return new_root
        if not isinstance(root, Node):
            raise Exception('传入的参数不是Node类型')
        Node.copy_tree(new_root, root)
        return new_root


if __name__ == '__main__':
    root = Node()
    root.insert('斗罗大陆之龙王传说', 1)
    root.insert('斗罗大陆之苏慕白', 2)
    root.insert('斗罗大陆之龙斗罗', 3)
    root.insert('叶非夜', 4)
    root.insert('叶非夜', 6)
    root.insert('匪我思存', 5)
    for node, dis in root.find('夜非夜', 2): print node.string, node.oinfo, dis
