#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import datetime
import pypinyin
from collections import deque
from pypinyin import lazy_pinyin

sys.path.append(os.path.dirname(os.path.split(os.path.realpath(__file__))[0]))

from lib import replacer
from lib import utils
from lib.utils import is_chinese
from lib.separater import Root as S_Root
from lib.replacer import Node
from lib.tokenizer import Root

from service import SER


NUM_1 = set(u'1234567890')
NUM_2 = set(u'一二三四五六七八九十')
NUM_3 = set(u'壹贰叁肆伍陆柒捌玖拾')
#            ⒍           ハ         з           O      ⒐         ニ         З
NUM_4 = set([u'\u248d', u'\u30cf', u'\u0437', u'O', u'\u2490', u'\u30cb', u'\u0417',
#             б           ろ          フ       ⒋          〇        l       ㄧ
             u'\u0431', u'\u308d', u'\u30d5', u'\u248b', u'\u3007', u'l', u'\u3127',
#            ⒉ 
             u'\u2489', u'o', u'\u2490', u'\u2491', u'\u2492', u'\u2493', u'\u2494',

             u'\u2495', u'\u2496', u'\u2497', u'\u2498', u'\u2499', u'\u2500', u'\u2502',

             u'\u25cb', u'\u043e', u'\u041e', u'\u2014', u''])
NUM_5 = set(u'⑴⑵⑶⑷⑸⑹⑺⑻⑼')
NUM_6 = set(u'①②③④⑤⑥⑦⑧⑨')
NUM_7 = set(u'㈠㈡㈢㈣㈤㈥㈦㈧㈨')
NUM_8 = set(u'ⅠⅡⅢⅣⅤⅥⅦⅧⅨ〇')
NUM_9 = set(u'⒈⒉⒊⒋⒌⒍⒎⒏⒐')
NUM_10 = set(u'陸彡仈勼')
NUM_11 = set(u'期寺衣把吧伞铃溜漆棋义久吧零舞散午灵留灵亿斯尔灵而漆扒琳琪耳期易丝武漆刘芭巴')
NUM_12 = set(u'凌铃龄伶澪靈亿依易咦异乙译弍两儿尔迩兒散叄馓弎私思撕厮斯似寺司泗舞无唔捂雾呜巫無留镏柳柒妻骑齐棋器启扒把巴叭玖酒久酒疚究旧')
NUM_13 = set(u'O０１２３４５６７８９〇')
NUM_14 = set(u'⒈⒉⒊⒋⒌⒍⒎⒏⒐')
NUM_15 = set(u'ЗЧЧЗ〇ЗЗЧI⒉I')
NUM_16 = set(u'尔扒医玲灵舞衣舞期泠散就奇唔酒溜')
NUM_17 = set(u'б⒉扒⒑Ο舞⒖')

NUM_SET = NUM_1 | NUM_2 | NUM_3 | NUM_4 | NUM_5 | NUM_6 | NUM_7 | NUM_8 | NUM_9 | NUM_10 |NUM_11 | NUM_12 | NUM_13 | NUM_14 | NUM_15 |NUM_16 |NUM_17
NUM_SET2 = NUM_1 | NUM_2 | NUM_3| NUM_4 | NUM_9 | NUM_13 | NUM_14 | NUM_15 | NUM_7 | NUM_6 |NUM_17

PINYIN_NUM = set([lazy_pinyin(word, style=pypinyin.NORMAL)[0] for word in u'一二三四五六七八九十零'])


def get_special_sentences():
    """ pass
    """
    special_senteces = []
    #for word_1 in [u'嗖', u'搜索', u'找', u'查找', u'搜', u'查', u'搜', u'叟索', u'请到' u'来到']:
    #    for word_2 in u'鮶桾捃裙峮群郡珺㪊':

    for word_1 in [u'搜索', u'查找', u'搜', u'搜', u'叟索', u'搜索支付宝', u'支付宝']:
        for word_2 in [u'立即领红包', u'领红包', u'立领红包', u'就能领红包', u'大红包',
                       u'红包', u'花呗红包', u'最多抢99元']:
            special_senteces.append(word_1 + '<8-15>' + word_2)

    special_senteces.append(u"打开支付宝搜索")
    special_senteces.append(u"支付宝搜索")
    return special_senteces


def get_qun_names():
    """ pass
    """
    special_senteces = []
    project_path = os.path.dirname(os.path.split(os.path.realpath(__file__))[0])
    with open(project_path + '/discern/data/group_names.txt', 'r') as file:
        for line in file.readlines():
            special_senteces.append(line.strip().decode('utf8'))
    return special_senteces


def get_num_count(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf8')
    global NUM_SET2
    num_count = 0
    for w in content:
        if w in NUM_SET2:
            num_count +=1
    return num_count


qun_name_root, is_init_qun_name_root = Root(), False

def init_qun_name_root():
    """ pass
    """
    global qun_name_root, is_init_qun_name_root
    if is_init_qun_name_root:
        return
    qun_name_senteces = get_qun_names()
    for qun_name in qun_name_senteces:
        qun_name_root.insert(qun_name)
    is_init_qun_name_root = True


def is_qun_name_match(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    global qun_name_root

    content = replace_special_char(content)

    init_qun_name_root()

    phrase_info_list = qun_name_root.get_phraseinfo_list(content)

    if phrase_info_list:
        return True
    return False


s_root, is_init_s_root = S_Root(), False

def init_s_root():
    """ pass
    """
    global s_root, is_init_s_root
    if is_init_s_root:
        return
    special_senteces = get_special_sentences()
    for sentence in special_senteces:
        s_root.insert(sentence)
    is_init_s_root = True


special_char_1 = set(u"""？。，；“”、""")
special_char_2 = set(u""" `,"?？.；，。‘ ·!、！》《〖〗”“~·【】[]：-=()（）)(’\n ` """)
def is_special_sentence_match(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf8')
    global s_root, is_init_s_root

    content = replace_special_char(content)

    init_s_root()
    phrases = s_root.get_cutinfo_list(content)

    if len(phrases):
        return True

    count = 0.0
    for phrase, len_p, other_info, start_idx, end_idx in phrases:
        for word in content[start_idx: end_idx]:
            if word in special_char_1:
                count += 0.5
            elif word in special_char_2:
                count += 1
            if count >= 2:
                return True
    return False


def get_continue_num2(content):
    """ 识别数字
    """
    global NUM_SET2

    if isinstance(content, str):
        content = content.decode('utf8')

    num_info_list, pre_index, min_dis, total, fix_length, num_total = [], -1, 1, 0, 2, 0
    for index, word in enumerate(content):
        if word in NUM_SET2:
            num_total += 1
            if pre_index == -1:
                pre_index, total = index, 1
            else:
                total += 1
            min_dis = 1
        else:
            if pre_index and min_dis >= 1 and not is_chinese(word):
                min_dis -= 1
            else:
                if total >= fix_length:
                    num_info_list.append((pre_index, index - 1, total, content[pre_index: index - 1 + min_dis]))
                pre_index, min_dis, total = -1, 1, 0
    if pre_index != -1 and total >= fix_length:
        num_info_list.append((pre_index, index - 2, total, content[pre_index:]))

    if not num_info_list or num_total < 6:
        return []

    new_content = utils.filter_special_char(content)
    idx_1 = new_content.find(u'数字')
    idx_2 = new_content.find(u'拼接')
    idx_3 = new_content.find(u'连接')
    idx_4 = new_content.find(u'群')
    idx_5 = new_content.find(u'号')

    if idx_1 > 0 or idx_2 > 0 or idx_3 > 0 or idx_4 >0 or idx_5 > 0:
        fix_length = 40
    else:
        fix_length = 10

    num_info_list_tmp, pre_num_info = [], (-2, -1, 0, "")
    for start_idx, end_idx, num_len, num_str in num_info_list:
        if 2 <= pre_num_info[2] <= 6 and 2 <= num_len <= 6 and start_idx - pre_num_info[0] <= 40:
            if num_len + pre_num_info[2] >= 7:
                num_info_list_tmp.append((pre_num_info[0], end_idx, num_len + pre_num_info[2], pre_num_info[3] + num_str))
                pre_num_info = (start_idx, end_idx, num_len, num_str)
            else:
                pre_num_info = (pre_num_info[0], end_idx, num_len + pre_num_info[2], pre_num_info[3] + num_str)
        elif num_len >= 7:
            num_info_list_tmp.append((start_idx, end_idx, num_len, num_str))
            pre_num_info = (start_idx, end_idx, num_len, num_str)
        else:
            pre_num_info = (start_idx, end_idx, num_len, num_str)
    return num_info_list_tmp


def get_continue_num(content):
    """ 识别数字
    """
    global NUM_SET, PINYIN_NUM

    if isinstance(content, str):
        content = content.decode('utf8')

    result, pre_index, min_dis, total, fix_length = [], -1, 2, 0, 7
    for index, word in enumerate(content):
        if word in NUM_SET:
            if pre_index == -1:
                pre_index, total = index, 1
            else:
                total += 1
            min_dis = 1
        else:
            if pre_index and min_dis >= 1 and not is_chinese(word):
                if word not in u'，,、. 。、':
                    min_dis -= 1
            else:
                if total >= fix_length:
                    result.append((pre_index, index - 1, total, content[pre_index: index - 1 + min_dis]))
                pre_index, min_dis, total = -1, 1, 0
    if pre_index != -1 and total >= fix_length:
        result.append((pre_index, index - 2, total, content[pre_index:]))
    return result


def recognition_continue_num(content, contents):
    """ 识别数字
    """
    global NUM_SET, PINYIN_NUM

    if isinstance(content, str):
        content = content.decode('utf8')

    result, pre_index, min_dis, total, fix_length = [], -1, 2, 0, 7
    for index, (word, py_note, py_initials, py_finals) in enumerate(contents):
        if word in NUM_SET or py_note[0] in PINYIN_NUM:
            if pre_index == -1:
                pre_index, total = index, 1
            else:
                total += 1
            min_dis = 1
        else:
            if pre_index and min_dis >= 1 and not is_chinese(word):
                if word not in u'，,、. 。、':
                    min_dis -= 1
            else:
                if total >= fix_length:
                    result.append((pre_index, index - 1, total, content[pre_index: index - 1 + min_dis]))
                pre_index, min_dis, total = -1, 1, 0
    if pre_index != -1 and total >= fix_length:
        result.append((pre_index, index - 2, total, content[pre_index:]))
    return result


qun_same_words = set(u'君鮶桾捃裙峮群郡珺㪊')
def get_qunkeyword_idx_list(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    qun_position_idx_list = []
    for idx, word in enumerate(content):
        if word in qun_same_words:
            qun_position_idx_list.append(idx)
    return qun_position_idx_list


INIT_WORD_ROOT, RELATEDWORD_ROOT = False, Root()
def init_related_word_root():
    """ pass
    """
    global INIT_WORD_ROOT, RELATEDWORD_ROOT
    RELATEDWORD_ROOT.insert(u'搜索', 1)
    RELATEDWORD_ROOT.insert(u'叟索', 1)
    RELATEDWORD_ROOT.insert(u'叟', 1)
    RELATEDWORD_ROOT.insert(u'嗖', 1)
    RELATEDWORD_ROOT.insert(u'搜', 1)
    RELATEDWORD_ROOT.insert(u'查找', 1)
    RELATEDWORD_ROOT.insert(u'查', 1)
    RELATEDWORD_ROOT.insert(u'察找', 1)
    RELATEDWORD_ROOT.insert(u'搜下', 1)

    RELATEDWORD_ROOT.insert(u'名字', 2)
    RELATEDWORD_ROOT.insert(u'铭', 2)
    RELATEDWORD_ROOT.insert(u'铭字', 2)
    RELATEDWORD_ROOT.insert(u'名称', 2)
    RELATEDWORD_ROOT.insert(u'铭称', 2)
    RELATEDWORD_ROOT.insert(u'名', 2)
    RELATEDWORD_ROOT.insert(u'号', 2)
    RELATEDWORD_ROOT.insert(u'銘', 2)
    RELATEDWORD_ROOT.insert(u'銘称', 2)

    RELATEDWORD_ROOT.insert(u'请到', 2)
    RELATEDWORD_ROOT.insert(u'请入', 2)
    
    for word_1 in u'加阱进':
        for word_2 in u'君鮶桾捃裙峮群郡珺㪊':
            RELATEDWORD_ROOT.insert(word_1 + word_2, 3)
            for word_3 in u'"?？.；，‘ ·!、！》《〖〗”“~·【】[]：-=()（）)(':
                RELATEDWORD_ROOT.insert(word_1 + word_3 + word_2, 3)

    for word_1 in [u'我们的', u'请到', u'到', u'来', u'欢迎来']:
        for word_2 in u'君鮶桾捃裙峮群郡珺㪊':
            RELATEDWORD_ROOT.insert(word_1 + word_2, 3)
            for word_3 in u'"?？.；，‘ ·!、！》《〖〗”“~·【】[]：-=()（）)(':
                RELATEDWORD_ROOT.insert(word_1 + word_3 + word_2, 3)

    for word_1 in [u'君交流', u'嗖', u'搜索', u'找', u'查找', u'搜', u'查', u'搜', u'叟索', u'请到', u'到', u'搜下']:
        for word_2 in u'鮶桾捃裙峮群郡珺㪊':
            RELATEDWORD_ROOT.insert(word_1 + word_2, 3)
            RELATEDWORD_ROOT.insert(word_2 + word_1, 3)
            for word_3 in u'"?？.；，‘ ·!、！》《〖〗”“~·【】[]：-=()（）)(':
                RELATEDWORD_ROOT.insert(word_1 + word_3 + word_2, 3)
                RELATEDWORD_ROOT.insert(word_2 + word_3 + word_1, 3)

    for word in u'鮶桾捃裙峮群郡珺㪊':
        RELATEDWORD_ROOT.insert(word, 4)

    for word in u""" `,"?？.；，。‘ ·!、！》《〖〗”“~·【】[]：-=()（）)(’\n ` """:
        RELATEDWORD_ROOT.insert(word, 5)

    for word_1 in [u'名称', u'名字', u'名', u'銘', u'号', u'銘称', u'铭字']:
        for word_2 in u'君鮶桾捃裙峮群郡珺㪊':
            RELATEDWORD_ROOT.insert(word_2 + word_1, 6)
            for word_3 in u'"?？.；，‘ ·!、！》《〖〗”“~·【】[]：-=()（）)(的 ':
                RELATEDWORD_ROOT.insert(word_2 + word_3 + word_1, 6)

    INIT_WORD_ROOT = True


def get_qunrelatedword_idx_list(content):
    """ pass
    """
    global INIT_WORD_ROOT, RELATEDWORD_ROOT
    if isinstance(content, str):
        content = content.decode('utf8')

    if not INIT_WORD_ROOT:
        init_related_word_root()

    phrase_info_list = RELATEDWORD_ROOT.get_spechar_phraseinfo_list(content)
    all_idx = set([index for phrase, p_len, o_info, index in phrase_info_list])
    o_phrase_info_list = RELATEDWORD_ROOT.get_phraseinfo_list(content)
    for o_phrase_info in o_phrase_info_list:
        if o_phrase_info[3] not in all_idx:
            phrase_info_list.append(o_phrase_info)
    phrase_info_list = sorted(phrase_info_list, key=lambda v:v[3])
    qunrelatedword_idx_list = []
    for phrase, p_len, o_info, index in phrase_info_list:
        #print phrase, index
        qunrelatedword_idx_list.append((index, o_info))
    return qunrelatedword_idx_list


qun_same_words = set(u'君鮶桾捃裙峮群郡珺㪊')
qun_ignore_words = set(u'逡纯唇纯淳春')
def recognition_qun(content, contents):
    """ 识别 群
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    result, note, initials, finals = [], u'qun', u'q', u'uen'

    pos = content.find('qun')
    if pos != -1:
        result.append((pos, 3, 0, u'que'))

    for index, (word, py_note, py_initials, py_finals) in enumerate(contents):
        if word in qun_ignore_words:
            continue

        if word in qun_same_words:
            result.append((index, 3, 0, word))

        if py_note and py_note[0] == note:
            result.append((index, 3, 0, word))

        if py_finals and py_finals[0] == finals:
            result.append((index, 1, 0, word))

        if py_initials and py_initials[0] == initials:
            result.append((index, 1, 0, word))

    return result


hao_same_words = set(u'号')
hao_ignore_words = set(u'好郝浩')
def recognition_hao(content, contents):
    """ 识别 号
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    result, note, initials, finals = [], u'hao', u'h', u'ao'

    pos = content.find('hao')
    if pos != -1:
        result.append((pos, 2, 1, u'hao'))

    for index, (word, py_note, py_initials, py_finals) in enumerate(contents):

        if word in hao_ignore_words:
            continue

        if word in hao_same_words:
            result.append((index, 2, 1, word))

        if py_note and py_note[0] == note:
            result.append((index, 1, 1, word))

        if py_finals and py_finals[0] == finals:
            result.append((index, 1, 1, word))

        if py_initials and py_initials[0] == initials:
            result.append((index, 1, 1, word))

    return result

jia_same_words = set(u'+加珈迦枷徍')
jia_ignore_words = set(u'京将就叫交近建接哎爱唉家')
def recognition_jia(content, contents):
    """ 识别 加
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    result, note, initials, finals = [], u'jia', u'j', u'ia'

    for index, (word, py_note, py_initials, py_finals) in enumerate(contents):
        if word in jia_ignore_words:
            continue

        if word in jia_same_words:
            result.append((index, 2, 2, word))

        if py_note and py_note[0] == note:
            result.append((index, 2, 2, word))

        if py_finals and py_finals[0] == finals:
            result.append((index, 1, 2, word))

        if py_initials and py_initials[0] == initials:
            result.append((index, 1, 2, word))

    return result


lai_same_words = set(u'来莱來徕')
lai_ignore_words = set(u'了啦李浪冷弄')
def recognition_lai(content, contents):
    """ 识别 来
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    result, note, initials, finals = [], u'lai', u'l', u'ai'

    for index, (word, py_note, py_initials, py_finals) in enumerate(contents):

        if word in lai_ignore_words:
            continue

        if word in lai_same_words:
            result.append((index, 2, 3, word))

        if py_note and py_note[0] == note:
            result.append((index, 2, 3, word))

        if py_finals and py_finals[0] == finals:
            result.append((index, 1, 3, word))

        if py_initials and py_initials[0] == initials:
            result.append((index, 1, 3, word))

    return result


jin_same_words = set(u'阱进')
jin_ignore_words = set(u'金紧今劲尽')
def recognition_jin(content, contents):
    """ 识别 进
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    result, note, initials, finals = [], u'jin', u'j', u'in'

    for index, (word, py_note, py_initials, py_finals) in enumerate(contents):
        if word in jin_ignore_words:
            continue

        if word in jin_same_words:
            result.append((index, 2, 4, word))

        if py_note and py_note[0] == note:
            result.append((index, 2, 4, word))

        if py_finals and py_finals[0] == finals:
            result.append((index, 1, 4, word))

        if py_initials and py_initials[0] == initials:
            result.append((index, 1, 4, word))

    return result


ma_same_words = set(u'玛码')
ma_ignore_words = set(u'麻吗妈骂么嘛')
def recognition_ma(content, contents):
    """ 识别 码
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    result, note = [], u'ma'

    for index, (word, py_note, py_initials, py_finals) in enumerate(contents):
        if word in ma_ignore_words:
            continue

        if word in ma_same_words:
            result.append((index, 2, 5, word))

        if py_note and py_note[0] == note:
            result.append((index, 2, 5, word))

    return result


def recognition_q(content):
    """ 识别 q
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    result = []

    pre_word = ''
    for index, word in enumerate(content):
        if word == 'q' or word == 'Q':
            if pre_word.lower() == word.lower():
                offset = 0
            else:
                offset = 1
            result.append((index, 2 + offset, 6, word))
        pre_word = word

    return result

sou_same_words = set(u'搜')
sou_ignore_words = set(u'')
def recognition_sou(content, contents):
    """ 识别 搜
    """
    if isinstance(content, str):
        content = content.decode('utf8')

    result = []
    for index, (word, py_note, py_initials, py_finals) in enumerate(contents):
        if word in sou_ignore_words:
            continue

        if word in sou_same_words:
            result.append((index, 2, 7, word))

    return result


def get_word_info(content, contents):
    """ pass
    """

    q_info = recognition_q(content)
    ma_info = recognition_ma(content, contents)
    qun_info = recognition_qun(content, contents)
    jia_info = recognition_hao(content, contents)
    hao_info = recognition_jia(content, contents)
    lai_info = recognition_lai(content, contents)
    jin_info = recognition_jin(content, contents)
    sou_info = recognition_sou(content, contents)

    pinyin_info = []
    pinyin_info.extend(qun_info)
    pinyin_info.extend(jia_info)
    pinyin_info.extend(hao_info)
    pinyin_info.extend(lai_info)
    pinyin_info.extend(ma_info)
    pinyin_info.extend(q_info)
    pinyin_info.extend(sou_info)

    pinyin_info = sorted(pinyin_info, key=lambda x: x[0])
    return pinyin_info


TREE, INIT = Node(), False

def init():
    """ pass
    """
    global TREE, INIT
    if INIT: return
    replacer.insert(TREE, u'qq游戏', '')
    replacer.insert(TREE, u'qq浏览器', '')
    replacer.insert(TREE, u'qq音乐', '')
    replacer.insert(TREE, u'qq阅读', '')
    replacer.insert(TREE, u'qq读书', '')
    replacer.insert(TREE, u'QQ游戏', '')
    replacer.insert(TREE, u'QQ音乐', '')
    replacer.insert(TREE, u'QQ阅读', '')
    replacer.insert(TREE, u'QQ读书', '')
    replacer.insert(TREE, u'可以', '')
    replacer.insert(TREE, u'群里等你', '加群')
    replacer.insert(TREE, u'羣', u'群')
    replacer.insert(TREE, u'阿拉伯数字', '')
    replacer.insert(TREE, u'替换', '')
    replacer.insert(TREE, u'数字', '')
    replacer.insert(TREE, u' ', '')
    replacer.insert(TREE, u'\n', '')
    replacer.insert(TREE, u'礻君', '群')
    replacer.insert(TREE, u'山君', '群')
    replacer.insert(TREE, u'可加', u'加')
    replacer.insert(TREE, u'扣', 'q')
    replacer.insert(TREE, u'蔻', 'q')
    replacer.insert(TREE, u'只要加', u'加')
    replacer.insert(TREE, r'\r\n', r'')
    replacer.insert(TREE, '\r\n', '')
    replacer.insert(TREE, '\n', '')
    replacer.insert(TREE, u'企鹅', 'QQ')
    replacer.insert(TREE, u'企鵝', 'QQ')
    replacer.insert(TREE, u'群耗', u'群号')
    replacer.insert(TREE, u'来一票', '')
    replacer.insert(TREE, u'来一次', '')
    replacer.insert(TREE, u'跑来', '')
    replacer.insert(TREE, u'走来', '')
    replacer.insert(TREE, u'来自', '')
    replacer.insert(TREE, u'加油', '')
    replacer.insert(TREE, u'码字', '')
    replacer.insert(TREE, u'红包群', '')
    replacer.insert(TREE, u'滚来滚去', '')
    replacer.insert(TREE, u'聚集地', u'群')
    replacer.insert(TREE, u'起名', u'')
    replacer.insert(TREE, u'女主名', u'')
    replacer.insert(TREE, u'男主名', u'')
    replacer.insert(TREE, u'主角名', u'')
    replacer.insert(TREE, u'男一名', u'')
    replacer.insert(TREE, u'女一名', u'')
    replacer.insert(TREE, u'君羊', u'群')
    replacer.insert(TREE, u'礻君', u'群')
    replacer.insert(TREE, u'鸡群', u'')
    replacer.insert(TREE, u'鸭群', u'')
    replacer.insert(TREE, u'鸟群', u'')
    replacer.insert(TREE, u'人群', u'')
    replacer.insert(TREE, u'这群可怜', u'')
    replacer.insert(TREE, u'君王', u'')
    replacer.insert(TREE, u'一群SB', u'')
    replacer.insert(TREE, u'一群sb', u'')
    replacer.insert(TREE, u'一群垃圾', u'')
    replacer.insert(TREE, u'一群傻叉', u'')
    replacer.insert(TREE, u'一群智障', u'')
    replacer.insert(TREE, u'一群白嫖', u'')
    replacer.insert(TREE, u'一群找存在感的', u'')
    replacer.insert(TREE, u'一群闲得无聊', u'')
    replacer.insert(TREE, u'qiu', u'Q')
    replacer.insert(TREE, u'群殴', u'')
    replacer.insert(TREE, u'看到', u'')
    replacer.insert(TREE, u'见到', u'')
    replacer.insert(TREE, u'听到', u'')
    replacer.insert(TREE, u'拿到', u'')
    replacer.insert(TREE, u'君子', u'')
    replacer.insert(TREE, u'群里作者', u'')
    replacer.insert(TREE, u'群海战术', u'')
    replacer.insert(TREE, u'想到', u'')
    replacer.insert(TREE, u'裙子', u'')
    replacer.insert(TREE, u'想不到', u'')
    replacer.insert(TREE, u'力口', u'加')
    replacer.insert(TREE, u'君羊', u'群')
    replacer.insert(TREE, u'找不到', u'')
    replacer.insert(TREE, u'君少', u'')
    replacer.insert(TREE, u'再到', u'')
    replacer.insert(TREE, u'驾到', u'')
    replacer.insert(TREE, u'君主', u'')
    replacer.insert(TREE, u'进群了好多小可爱', u'')
    replacer.insert(TREE, u'郡主', u'')
    replacer.insert(TREE, u'qun', u'群')
    replacer.insert(TREE, u'表白作者君', u'')
    replacer.insert(TREE, u'与作者君', u'')
    replacer.insert(TREE, u'同作者君', u'')
    replacer.insert(TREE, u'是', u'')
    replacer.insert(TREE, u'郡守', u'')
    replacer.insert(TREE, u'作者君', u'')
    replacer.insert(TREE, u'名声', u'')
    replacer.insert(TREE, u'名气', u'')
    replacer.insert(TREE, u'名望', u'')
    replacer.insert(TREE, u'威名', u'')
    replacer.insert(TREE, u'声名', u'')
    replacer.insert(TREE, u'遇到', u'')
    replacer.insert(TREE, u'^_^', u'')
    replacer.insert(TREE, u'群里', u'')
    replacer.insert(TREE, u'quot', u'')
    replacer.insert(TREE, u'Ｑ', u'QQ')
    replacer.insert(TREE, u'叩叩', u'QQ')
    replacer.insert(TREE, u'￣ω￣=', u'')
    replacer.insert(TREE, u'5201314', u'')
    replacer.insert(TREE, u'1314', u'')
    replacer.insert(TREE, u'2018', u'')
    replacer.insert(TREE, u'2019', u'')
    replacer.insert(TREE, u'2020', u'')
    replacer.insert(TREE, u'2017', u'')
    for i in xrange(0, 200):
        replacer.insert(TREE, u'[fn=%s]' % i, u'')
        replacer.insert(TREE, u'[emot=default,%s]' % i, u'')
    for i in xrange(1, 100):
        replacer.insert(TREE, u'&#0%s' % i, '')
    INIT = True


def replace_special_char(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf-8')
    init()
    while True:
        ori_content = content
        content = replacer.replace(TREE, content)
        if len(ori_content) == len(content):
            return content


def check_condition(num_info, info, sum_val, queue, total_dis):
    """ pass
    """
    max_num = 5
    if sum_val >= 7:
        return True

    if not num_info:
        return False

    if (abs(num_info[0] - info[0]) <= 25 or abs(num_info[1] - info[0]) <= 25):
        if sum_val >= 6:
            if len(queue) <= 2:
                return True

    len_deque = len(queue)

    if abs(num_info[0] - info[0]) <= max_num or abs(num_info[1] - info[0]) <= max_num:
        if 3 <= sum_val and len_deque <= 1:
            return True
        if 4 <= sum_val and len_deque <= 2:
            return True
        if 5 <= sum_val and len_deque <= 3:
            return True

    return False


def have_qq_group_num(continue_nums, pinyin_info):
    """ pass
    """
    if continue_nums:
        num_info, info_index = continue_nums[0], 0
    else:
        num_info, info_index = None, -1

    max_dis = 5
    sum_val, total_dis, deque_obj, pre_info = 0, 0, deque(), None
    for info in pinyin_info:
        if pre_info and check_condition(num_info, pre_info, sum_val, deque_obj, total_dis):
            return True

        #print sum_val, info[3], info, len(deque_obj), '---'
        if pre_info and info[0] - pre_info[0] > max_dis:
            sum_val = info[1]
            deque_obj.clear()
            deque_obj.append(info[1])
            total_dis = 0
        else:
            if not pre_info or pre_info[2] != info[2]:
                sum_val += info[1]
                deque_obj.append(info[1])
            else:
                tmp_weight = deque_obj.pop()
                max_weight = max(tmp_weight, info[1])
                sum_val = sum_val - tmp_weight + max_weight
                deque_obj.append(max_weight)
            if len(deque_obj) >= 3:
                sum_val -= deque_obj.popleft()
        #print sum_val, info[3], info, len(deque_obj), '+++'

        pre_info = info
        if num_info and info[0] - num_info[1] > max_dis:
            if info_index + 1 < len(continue_nums):
                num_info, info_index = continue_nums[info_index + 1], info_index + 1
            else:
                num_info, info_index = num_info, info_index

    if pre_info and check_condition(num_info, pre_info, sum_val, deque_obj, total_dis):
        return True

    return False


def get_qq_group(continue_nums, pinyin_info):
    """ pass
    """
    if continue_nums:
        num_info, info_index = continue_nums[0], 0
    else:
        num_info, info_index = None, -1

    max_dis = 3
    sum_val, total_dis, deque_obj, pre_info = 0, 0, deque(), None
    for info in pinyin_info:
        # print sum_val, info[3], info, len(deque_obj), '---'
        if pre_info and check_condition(num_info, pre_info, sum_val, deque_obj, total_dis):
            return num_info
        if pre_info and info[0] - pre_info[0] > max_dis:
            sum_val = info[1]
            deque_obj.clear()
            deque_obj.append(info[1])
            total_dis = 0
        else:
            if not pre_info or pre_info[2] != info[2]:
                sum_val += info[1]
                deque_obj.append(info[1])
            else:
                tmp_weight = deque_obj.pop()
                max_weight = max(tmp_weight, info[1])
                sum_val = sum_val - tmp_weight + max_weight
                deque_obj.append(max_weight)
            if len(deque_obj) > 3:
                sum_val -= deque_obj.popleft()
        # print sum_val, info[3], info, len(deque_obj), '+++'

        pre_info = info
        if num_info and info[0] - num_info[1] > max_dis:
            if info_index + 1 < len(continue_nums):
                num_info, info_index = continue_nums[info_index + 1], info_index + 1
            else:
                num_info, info_index = None, -1

    if pre_info and check_condition(num_info, pre_info, sum_val, deque_obj, total_dis):
        return num_info


def have_qq_group_name(content, qun_position_idx_list, qun_relatedword_idx_list):
    """ pass
    """
    for qun_pos_idx in qun_position_idx_list:
        is_get_find, is_get_name, is_get_jinqun, is_get_qun, is_get_special, special_math, is_get_ming = False, False, False, False, False, False, False
        score, pre_special_char_idx = 2, -1000
        for idx, (related_idx, o_info) in enumerate(qun_relatedword_idx_list):
            if o_info == 4 and related_idx == qun_pos_idx:
                continue
            if o_info == 1 and not is_get_find and related_idx >= qun_pos_idx - 4 and related_idx >= qun_pos_idx - 5:
                is_get_find = True
                #score += 2
                score += 1

            if o_info == 2 and not is_get_name and related_idx <= qun_pos_idx + 4 and related_idx >= qun_pos_idx - 5:
                is_get_name = True
                #score += 2
                score += 1

            if o_info == 3 and qun_pos_idx - 30 <= related_idx and qun_pos_idx + 30 >= related_idx:
                is_get_jinqun = True
                score += 4

            if o_info == 4 and qun_pos_idx - 20 <= related_idx and qun_pos_idx + 20 >= related_idx:
                is_get_qun = True
                score += 1

            if o_info == 5  and not is_get_special and qun_pos_idx - 20 <= related_idx and qun_pos_idx + 20 >= related_idx:
                is_get_special = True
                score += 1

            if o_info == 5 and not special_math:
                if related_idx - pre_special_char_idx <= 15  and related_idx - pre_special_char_idx > 3:
                    if qun_pos_idx - 20 <= related_idx and qun_pos_idx + 20 >= related_idx:
                        special_math = True
                        score += 1

            if o_info == 6  and not is_get_ming and qun_pos_idx + 5 >= related_idx:
                is_get_ming = True
                score += 3

            if o_info == 5:
                pre_special_char_idx = related_idx

        if not special_math and pre_special_char_idx + 15 >= len(content) and len(content) - pre_special_char_idx > 3:
            special_math = True
            score += 1

        #print content[max(0, qun_pos_idx-25): qun_pos_idx+25], '\n', is_get_find, is_get_name, is_get_jinqun, is_get_qun, is_get_ming, is_get_special, special_math, score

        if score >= 8: return True
    return False


def have_qq_group_name_process(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf-8')

    content = replace_special_char(content)

    qun_position_idx_list = get_qunkeyword_idx_list(content)

    if not qun_position_idx_list:
        return False

    qun_relatedword_idx_list = get_qunrelatedword_idx_list(content)

    #print content, '\n', qun_position_idx_list, '\n', qun_relatedword_idx_list

    return have_qq_group_name(content, qun_position_idx_list, qun_relatedword_idx_list)


def have_qq_group_num_process(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf-8')

    content = utils.filter_special_char(content)

    content = replace_special_char(content)

    contents = SER.pinyin.get_pinyin(content)


    continue_nums = recognition_continue_num(content, contents)

    if not continue_nums:
        return False

    pinyin_info = get_word_info(content, contents)


    #print content, '\n', continue_nums, '\n', pinyin_info 

    return have_qq_group_num(continue_nums, pinyin_info)


def have_chatgroup_process(content):
    """ pass
    """
    if is_qun_name_match(content):
        return True

    if is_special_sentence_match(content):
        return True

    content = replace_special_char(content)

    #if have_qq_group_name_process(content):
    #    return True

    if have_qq_group_num_process(content):
        return True

    return False


def get_chatgroup_info(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf-8')

    content = utils.filter_special_char(content)

    content = replace_special_char(content)

    contents = SER.pinyin.get_pinyin(content)

    continue_nums = recognition_continue_num(content, contents)
    if not continue_nums:
        return None, None

    content_info = get_word_info(content, contents)

    # print content, '\n', continue_nums, '\n', content_info

    num_info = get_qq_group(continue_nums, content_info)
    if num_info:
        return content[max(0, num_info[0] - 10): min(len(content), num_info[1] + 10)], num_info[3]
    return None, None

#print get_continue_num2("放假啊开12始的23就发生的空间发放假，开始的234, 放假啊是的12345677")
