#!/usr/bin/env python
# -*- coding: utf-8 -*-

__author__ = 'wulangzhou'


SPECIAL_CHAR = set(u""" ,"?？.。，；‘ ·!、！》《<>〖”“~·【】[]：-=()（）’\n ` """)


def is_chinese(cha):
    """ 判读一个字符是否汉字
    """
    if isinstance(cha, str):
        cha = cha.decode('utf-8')
    return u'\u4e00' <= cha <= u'\u9fa5'


class Node(object):
    """构造一个类用来保存树的节点信息
    """
    def __init__(self):
        """
        构造函数初始化
        """
        self.phrase = ''
        self.in_num = 0
        self.len_phrase = None
        self.next_node = {}
        self.other_info = None


class Root(object):
    """构造一个根节点
    """
    def __init__(self):
        """ 构造函数初始化
        """
        self.root = Node()
        self.first = [False] * 71000

    def insert(self, phrase, other_info=None):
        """从根节点插入一个词组
        """
        if isinstance(phrase, str):
            phrase = phrase.decode('utf-8')

        p = self.root
        for index, word in enumerate(phrase):
            ord_num = ord(word)

            if ord_num >= 71000:
                return

            if index == 0:
                self.first[ord_num] = True

            if not p.next_node.get(ord_num):
                p.next_node[ord_num] = Node()

            p.in_num += 1
            p = p.next_node[ord_num]

        if not p.phrase:
            p.phrase = phrase
            p.len_phrase = len(phrase)
            p.other_info= other_info

    def get_phraseinfo_list(self, content):
        """ pass
        """
        if isinstance(content, str):
            content = content.decode('utf-8')

        content = [word for word in content]

        index, len_content, phrase_infos = 0, len(content), []
        while index < len_content:
            ord_num = ord(content[index])
            if ord_num > 71000 or not self.first[ord_num]:
                index += 1
            else:
                p, max_match_phrase = self.root, None
                for j in xrange(index, len_content):
                    ord_num = ord(content[j])
                    if not p.next_node.get(ord_num):
                        break
                    p = p.next_node[ord_num]
                    if p.phrase:
                        max_match_phrase = (p, index + p.len_phrase - 1)
                if max_match_phrase:
                    phrase_infos.append(max_match_phrase)
                    index += max_match_phrase[0].len_phrase
                else:
                    index += 1
        return [(info[0].phrase, info[0].len_phrase, info[0].other_info, info[1]) for info in phrase_infos]

    def get_all_phraseinfo_list(self, content):
        """ pass
        """
        if isinstance(content, str):
            content = content.decode('utf-8')

        content = [word for word in content]

        index, len_content, phrase_infos = 0, len(content), []
        while index < len_content:
            ord_num = ord(content[index])
            if ord_num > 71000 or not self.first[ord_num]:
                index += 1
            else:
                p, max_match_phrase = self.root, None
                for j in xrange(index, len_content):
                    ord_num = ord(content[j])
                    if not p.next_node.get(ord_num):
                        break
                    p = p.next_node[ord_num]
                    if p.phrase:
                        phrase_infos.append((p, index + p.len_phrase - 1))
                index += 1
        return [(info[0].phrase, info[0].len_phrase, info[0].other_info, info[1]) for info in phrase_infos]

    def get_phrase_info(self, content, default=None):
        """ pass
        """
        if isinstance(content, str):
            content = content.decode('utf-8')

        content = [word for word in content]
        index, len_content, phrase_infos, p = 0, len(content), [], self.root
        while index < len_content:
            ord_num = ord(content[index])
            if ord_num > 71000: return default
            if not p.next_node.get(ord_num):
                return default
            p = p.next_node[ord_num]
            index += 1
        if p.other_info is not None:
            return p.other_info
        else:
            return default

    def get_spechar_phraseinfo_list(self, content):
        """ pass
        """
        if isinstance(content, str):
            content = content.decode('utf-8')

        content = [word for word in content]

        index, len_content, phrase_infos = 0, len(content), []
        while index < len_content:
            ord_num = ord(content[index])
            if ord_num > 71000 or not self.first[ord_num]:
                index += 1
            else:
                p, count, max_match_phrase = self.root, 0, None
                for j in xrange(index, len_content):
                    count += 1
                    ord_num = ord(content[j])
                    if content[j] in SPECIAL_CHAR or not is_chinese(content[j]):
                        continue
                    if not p.next_node.get(ord_num):
                        break
                    p = p.next_node[ord_num]
                    if p.phrase:
                        max_match_phrase = (p, index + count - 1)
                if max_match_phrase:
                    phrase_infos.append(max_match_phrase)
                    index += max_match_phrase[0].len_phrase
                else:
                    index += 1
        return [(info[0].phrase, info[0].len_phrase, info[0].other_info, info[1]) for info in phrase_infos]

    def get_spliter_phrases_info(self, content):
        """ pass
        """
        if isinstance(content, str):
            content = content.decode('utf-8')

        content = [word for word in content]

        index, len_content, phrase_list, phrase_str, phrase_idx = 0, len(content), [], "", 0
        while index < len_content:
            ord_num = ord(content[index])
            if ord_num > 71000 or not self.first[ord_num]:
                phrase_str += content[index]
                if len(phrase_str) == 1: phrase_idx = index
                index += 1
            else:
                p, count, max_match_phrase = self.root, 0, None
                for j in xrange(index, len_content):
                    count += 1
                    ord_num = ord(content[j])
                    #if content[j] in SPECIAL_CHAR or not is_chinese(content[j]):
                    #    continue
                    if not p.next_node.get(ord_num):
                        break
                    p = p.next_node[ord_num]
                    if p.phrase:
                        max_match_phrase = (p, index + count - 1)
                if max_match_phrase:
                    if len(phrase_str) != 0: phrase_list.append((phrase_str, phrase_idx))
                    phrase_str, phrase_idx = "", 0
                    index += max_match_phrase[0].len_phrase
                else:
                    phrase_str += content[index]
                    if len(phrase_str) == 1: phrase_idx = index
                    index += 1

        if len(phrase_str) != 0: phrase_list.append((phrase_str, phrase_idx))
        return phrase_list

    def get_spliter_phrases(self, content):
        """ pass
        """
        phrases_info = self.get_spliter_phrases_info(content)
        return [phrase for phrase, pos in phrases_info]

    def print_tree(self):
        """ pass
        """
        all_sons =[self.root]
        while all_sons:
            new_all_son = []
            for son in all_sons:
                for k, v in son.next_node.items():
                    print unichr(k), v
                    new_all_son.append(v)
                print ''
            all_sons = new_all_son

    def get_keys(self):
        """ pass
        """
        keys, all_sons = [], [self.root]
        while all_sons:
            tmp_sons = []
            for son in all_sons:
                if son.phrase != '':
                    keys.append(son.phrase)
                for k, v in son.next_node.items():
                    tmp_sons.append(v)
            all_sons = tmp_sons
        return keys


Tokenizer = Root

if __name__ == '__main__':
    root = Root()
    root.insert('没', set(['1', 2]))
    root.insert('　', set())
    root.insert(' ', set())
    root.insert('吴浪', set([2]))
    root.insert('哈哈')
    root.insert('吴浪舟', set([3]))
    root.insert('吴2')
    root.insert('1234')
    root.insert('134')
    root.insert('111')
    #for phrase in root.get_spliter_phrases("这是吴浪舟第一次哈哈大笑"): print phrase
    #for phrase in root.get_spliter_phrases("快穿   心机boss日日撩"): print phrase
    #for phrase in root.get_spliter_phrases("这是吴舟第一次大笑"): print phrase
    #for key in root.get_keys(): print key
    #for phrase, len_p, other_info, index in root.get_phraseinfo_list('　林嘉歌没看'): print phrase, index
    c = set()
    info = root.get_phrase_info('吴浪舟')
    c = c | info
    print info
    info = root.get_phrase_info('吴浪舟')
    print info
    #for phrase, len_p, other_info, index in root.get_all_phraseinfo_list('吴浪舟'): print phrase, index
