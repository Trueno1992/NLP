#!/usr/bin/env python
# -*- coding: utf-8 -*-

__author__ = 'wulangzhou'


import copy
from Queue import Queue


class T_Node(object):
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

class T_Root(object):
    """构造一个根节点
    """
    def __init__(self):
        """ 构造函数初始化
        """
        self.root = T_Node()
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
                p.next_node[ord_num] = T_Node()
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


class Node(object):
    """构造一个类用来保存树的节点信息
    """
    def __init__(self):
        """ 构造函数初始化
        """
        self.phrase = ''
        self.phrase_list = []
        self.in_num = 0
        self.len_phrase = None
        self.word_node_dict = {}
        self.num_words_dict = {}
        self.other_info = None


class Root(object):
    """ 构造一个根节点
    """
    def __init__(self):
        """ 构造函数初始化
        """
        self.root = Node()
        self.t_root = T_Root()

    def insert(self, phrase, other_info=None):
        """从根节点插入一个词组
        """
        if isinstance(phrase, str):
            phrase = phrase.decode('utf-8')

        words = [word for word in phrase]
        index, len_words, p, t_string, max_t_string = 0, len(words), self.root, '', ''
        while index < len_words:
            find_1, find_2, find_3, idx, num_str = False, False, False, index + 1, ''
            if words[index] == '<':
                while idx < len_words:
                    if words[idx] >= '0' and words[idx] <= '9':
                        num_str += words[idx]
                        find_1 = True
                    elif words[idx] == '-':
                        num_str += words[idx]
                        find_2 = True
                    elif words[idx] == '>':
                        find_3  = True
                        break
                    else:
                        break
                    idx += 1
            if find_1 and find_2 and find_3:
                index = idx + 1 
                nums = num_str.split('-')
                if len(nums) != 2:
                    raise Exception('num_str=%s, error' % num_str)
                num_begin, num_end = int(nums[0]), int(nums[1])
                if len(t_string) > len(max_t_string):
                    max_t_string = t_string
                t_string = ''
            else:
                num_begin, num_end = 0, 0
                t_string += words[index]

            if index >= len_words:
                break

            word = words[index]

            num_word_set = p.num_words_dict.get((num_begin, num_end), set())
            num_word_set.add(word)
            p.num_words_dict[(num_begin, num_end)] = num_word_set

            next_node = p.word_node_dict.get(word)
            if not next_node:
                next_node = Node()
                p.word_node_dict[word] = next_node

            p.in_num += 1
            index += 1
            p = p.word_node_dict[word]

        if len(t_string) > len(max_t_string):
            max_t_string = t_string

        if max_t_string:
            self.t_root.insert(max_t_string)

        if not p.phrase:
            p.phrase = phrase
            p.len_phrase = len(phrase)
            p.other_info= other_info
        p.phrase_list.append(phrase)

    def get_math_string_list(self, content):
        """ pass
        """
        if isinstance(content, str):
            content = content.decode('utf-8')

        if not self.t_root.get_phraseinfo_list(content):
            return []

        queue = Queue() 
        queue.put((0, 0, self.root, None, [], 1))

        content = [word for word in content]
        len_content, result_info_list, begin_idx_over, tag_over = len(content), [], set([0]), set()
        max_tag = 1
        while queue.qsize() != 0:
            queue_size = queue.qsize()
            for idx in xrange(queue_size):
                ori_index, end_index, f_root, best_info, math_string_list, tag = queue.get()
                is_get_next_node = False
                # print ori_index, end_index
                for (num_begin, num_end), words in f_root.num_words_dict.items():
                    # print ori_index, end_index, content[end_index], num_begin, num_end
                    for off_num in xrange(num_begin, num_end + 1):
                        off_index = end_index + off_num
                        if off_index < len_content:
                            n_word = content[off_index]
                        else:
                            continue
                        if n_word not in words:
                            continue
                        next_node = f_root.word_node_dict[n_word]
                        tmp_math_string_list = copy.deepcopy(math_string_list)
                        tmp_tag = tag
                        if off_num != 0:
                            tmp_math_string_list.append(''.join(content[end_index: end_index + off_num]))
                            tmp_tag <<= 1
                        max_tag = max(tmp_tag, max_tag)
                        if next_node.phrase:
                            best_info = (next_node.phrase, next_node.len_phrase, next_node.other_info, ori_index, off_index, tmp_math_string_list, tmp_tag)
                        is_get_next_node = True
                        queue.put((ori_index, off_index + 1, next_node, best_info, tmp_math_string_list, tmp_tag))
                if not is_get_next_node:
                    #if best_info: print best_info[3], best_info[4], best_info[6], best_info[6] not in tag_over
                    if best_info and best_info[0] + str(end_index) not in tag_over:
                        result_info_list.append(best_info)
                        tag_over.add(best_info[0] + str(end_index))
                    if ori_index + 1 < len_content and ori_index + 1 not in begin_idx_over:
                        queue.put((ori_index + 1, ori_index + 1, self.root, None, [], max_tag))
                        begin_idx_over.add(ori_index + 1)
            # print ''
        return result_info_list


    def get_cutinfo_list(self, content):
        """ pass
        """
        if isinstance(content, str):
            content = content.decode('utf-8')
        #content = '^' + content

        if not self.t_root.get_phraseinfo_list(content):
            return []

        queue = Queue() 
        queue.put((0, 0, self.root, None))

        end_idx_over = set()
        content = [word for word in content]
        len_content, result_info_list, begin_idx_over = len(content), [], set([0])
        while queue.qsize() != 0:
            queue_size = queue.qsize()
            for idx in xrange(queue_size):
                ori_index, end_index, f_root, best_info = queue.get()
                is_get_next_node = False
                # print ori_index, end_index
                for (num_begin, num_end), words in f_root.num_words_dict.items():
                    # print ori_index, end_index, content[end_index], num_begin, num_end
                    for off_num in xrange(num_begin, num_end + 1):
                        off_index = end_index + off_num
                        if off_index < len_content:
                            n_word = content[off_index]
                        else:
                            continue
                        if n_word not in words:
                            continue
                        next_node = f_root.word_node_dict[n_word]
                        if next_node.phrase:
                            best_info = (next_node.phrase, next_node.len_phrase, next_node.other_info, ori_index, off_index)
                        is_get_next_node = True
                        queue.put((ori_index, off_index + 1, next_node, best_info))

                if not is_get_next_node:
                    if best_info:
                        result_info_list.append(best_info)
                    if ori_index + 1 < len_content and ori_index + 1 not in begin_idx_over:
                        queue.put((ori_index + 1, ori_index + 1, self.root, None))
                        begin_idx_over.add(ori_index + 1)
            # print ''
        return result_info_list


def get_math_string_list_test():
    """ pass
    """
    root = Root()
    root.insert('非常')
    root.insert('有')
    root.insert('代入感')
    root.insert('就像')
    root.insert('，')
    root.insert('^自己')
    root.insert('身<0|2>残志坚')
    root.insert('身<1-2>临其境')
    root.insert('身<1-2>残志坚')
    root.insert('<1>')
    root.insert('<1')
    root.insert('<2>的文笔好好呀')
    root.insert('？')
    root.insert('?')
    root.insert('这本书和<2-20>是我看过的')
    root.insert('大大的文笔比<2-15>要好')
    root.insert('<2-5>的文笔')
    root.insert('<2-15>已经新鲜出炉，<2-5>支持')
    root.insert('可以和<2-3>一样')
    #phrases = root.get_math_string_list('叶子的文笔很好')
    #phrases = root.get_math_string_list('这本书和最佳朋友圈是我看过的最好的两本书')
    #phrases = root.get_math_string_list('最佳朋友圈已经新鲜出炉，望大家支持')
    phrases = root.get_math_string_list('作者的文笔,可以和叶子一样')
    if phrases:
        for phrase, len_p, other_info, start_idx, end_idx, math_string_list, tag in phrases:
            print phrase, start_idx, end_idx, tag
            for string in math_string_list:
                print string
            print ''

def get_cutinfo_list_test():
    """ pass
    """
    root = Root()
    root.insert('非常')
    root.insert('有')
    root.insert('代入感')
    root.insert('就像')
    root.insert('，')
    root.insert('^自己')
    root.insert('身<0|2>残志坚')
    root.insert('身<1-2>临其境')
    root.insert('身<1-2>残志坚')
    root.insert('<1>')
    root.insert('<1')
    root.insert('<2>的文笔好好呀')
    root.insert('？')
    root.insert('?')
    root.insert('这本书和<2-20>是我看过的')
    root.insert('大大的文笔比<2-15>要好')
    root.insert('<2-5>的文笔很好')
    root.insert('<2-15>已经新鲜出炉，<2-5>支持')
    root.insert('可以和<2-2>一样写出这么细腻感人的文章')
    phrases = root.get_cutinfo_list('可以和叶子一样写出这么细腻感人的文章')
    if phrases:
        for phrase, len_p, other_info, start_idx, end_idx in phrases:
            print phrase, start_idx, end_idx


if __name__ == '__main__':
    get_math_string_list_test()
    #get_cutinfo_list_test()
