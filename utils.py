#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import time
import datetime
import logging
import urllib
import urllib2
import urlparse
import ctypes
import traceback
from functools import wraps
from ctypes import c_char_p, c_int, c_longlong


BRACKETS_START = set(u'(（【[〖《')
BRACKETS_END = set(u')）】]〗》')
def remove_brackets_info(content):
    """ 去掉括号里面的内容: cc（abd）dd -> ccdd
                            cc(ab(d))kk -> cckk
    """
    content = to_unicode(content)
    tmp, bracket_in = "", 0
    for c in content:
        if c in BRACKETS_START:
            bracket_in += 1
        if bracket_in == 0:
            tmp += c
        if c in BRACKETS_END:
            bracket_in = max(0, bracket_in - 1)
    return tmp


def is_english(cha):
    """ 判断一个字符是否英文
    """
    if isinstance(cha, str):
        cha = cha.decode('utf-8')
    return 'a' <= cha <= 'z' or 'A' <= cha <= 'Z'


def is_chinese(cha):
    """ 判读一个字符是否汉字
    """
    if isinstance(cha, str):
        cha = cha.decode('utf-8')
    return u'\u4e00' <= cha <= u'\u9fa5'


def contain_chinese(content):
    """ 是否包含中文
    """
    if isinstance(content, str):
        content = content.decode('utf-8')
    for word in content:
        if is_chinese(word):
            return True
    return False


def get_chinese(content):
    """ pass
    """
    if isinstance(content, str):
        content = content.decode('utf-8')

    string = ""
    for i in content:
        if is_chinese(i):
            string += i
    return string


def count_chinese(content):
    """ 是否包含中文
    """
    if isinstance(content, str):
        content = content.decode('utf-8')
    count = 0
    for word in content:
        if is_chinese(word):
            count += 1
    return count


SPECIAL_CHAR = set(u"""-(^_^))￣ω￣=,"?？.。，；‘ ·!、！》《”“~·【】[]：≡;〖〗-=()（）<#≦’&\n ` ·~￣ω=""")
def get_puncutation_count(content):
    """ pass
    """
    global SPECIAL_CHAR
    if isinstance(content, str):
        content = content.decode('utf-8')
    puncutation_count = 0
    for w in content:
        if w in SPECIAL_CHAR or not is_chinese(w):
            puncutation_count += 1
    return puncutation_count


def filter_special_char(content):
    """ pass
    """
    global SPECIAL_CHAR
    if isinstance(content, str):
        content = content.decode('utf-8')

    res = ''
    for word in content:
        if word not in SPECIAL_CHAR:
            res = '%s%s' % (res, word)
    return res


def get_fix_edit_dis(content_a, content_b):
    """ pass
    """
    if isinstance(content_a, str):
        content_a = content_a.decode('utf-8')
    if isinstance(content_b, str):
        content_b = content_b.decode('utf-8')

    return max(len(content_a), len(content_b))


def get_edit_distance(content_a, content_b):
    """ pass
    """
    import Levenshtein
    content_a = to_unicode(content_a)
    content_b = to_unicode(content_b)
    return Levenshtein.distance(content_a, content_b)


def is_same(content_a, content_b, fix_dis=None, dis_rate=None):
    """ pass
    """
    import Levenshtein
    content_a = filter_special_char(content_a)
    content_b = filter_special_char(content_b)

    if isinstance(content_a, unicode):
        content_a = content_a.encode('utf-8')
    if isinstance(content_b, unicode):
        content_b = content_b.encode('utf-8')

    if len(content_a) == 0 and len(content_b) == 0:
        return True

    if dis_rate:
        max_length = get_fix_edit_dis(content_a, content_b) * 1.0
        return Levenshtein.distance(content_a, content_b) * 0.3 / (max_length) <= dis_rate

    if not fix_dis:
        fix_dis = get_fix_edit_dis(content_a, content_b) / 10 * 2

    return Levenshtein.distance(content_a, content_b) * 1.0 / 3 <= fix_dis


def get_max_same_str(content_a, content_b):
    """ pass
    """
    content_a = to_unicode(content_a)
    content_b = to_unicode(content_b)

    pre_idx, same_str, max_str = 0, "", ""
    for word_a in content_a:
        idx = content_b.find(word_a, pre_idx)
        if idx - 1 == -1 or idx - 1 == pre_idx:
            same_str += word_a
            pre_idx = idx
        else:
            if len(same_str) > len(max_str):
                max_str = same_str
            pre_idx, same_str = 0, ""
    if len(same_str) > len(max_str):
        max_str = same_str
    return max_str


def to_str(s, encoding="utf8"):
    '''to string'''
    if isinstance(s, unicode):
        return s.encode(encoding)
    return s


def to_unicode(data):
    """
    """
    if isinstance(data, str):
        data = data.decode('utf-8')
        #try:
        #    data = data.decode('utf-8')
        #except:
        #    data = data.decode('gbk')
    elif isinstance(data, list):
        for index, i in enumerate(data):
            data[index] = to_unicode(i)
    elif isinstance(data, dict):
        for key, val in data.items():
            data[to_unicode(key)] = to_unicode(val)
    elif isinstance(data, int):
        data = str(data)
    elif isinstance(data, float):
        data = str(data)
    return data


def to_utf8(data):
    """
    """
    if isinstance(data, unicode):
        try:
            data = data.encode('utf-8')
        except:
            data = data.decode('gbk').encode('utf-8')
    elif isinstance(data, list):
        for index, i in enumerate(data):
            data[index] = to_utf8(i)
    elif isinstance(data, dict):
        for key, val in data.items():
            data[to_utf8(key)] = to_utf8(val)
    return data


def open_excel(save_path):
    """ pass
    """
    records = Excel().load_file(save_path)
    for record in records:
        for k, v in record.items():
            print k, v
        print ''


def url_add_params(url, escape=True, **params):
    """
    add new params to given url
    """
    pr = urlparse.urlparse(url)
    query = dict(urlparse.parse_qsl(pr.query))
    query.update(params)
    prlist = list(pr)
    if escape:
        prlist[4] = urllib.urlencode(query)
    else:
        prlist[4] = '&'.join(['%s=%s' % (k, v) for k, v in query.items()])
    return urlparse.ParseResult(*prlist).geturl()


sentence_chinese_punctuation = u'，。！？；：…\n'
sentence_english_punctuation = u',.!?;:'
sentence_punctuation_set = set(sentence_chinese_punctuation + sentence_english_punctuation)
def get_sentence_list(paragraph):
    """ return list(unicode(sentence),)
    """
    if not isinstance(paragraph, basestring):
        raise Exception('def get_sentence(paragraph)的paragraph参数必须是字符串')
    if isinstance(paragraph, str):
        paragraph = paragraph.decode('utf-8')
    sentence_list, sentence_str = [], ''

    for word in paragraph:
        if word in sentence_punctuation_set:
            if sentence_str != '':
                sentence_list.append(sentence_str + word)
            sentence_str = ''
        else:
            sentence_str += word
    if sentence_str:
        sentence_list.append(sentence_str)
    return sentence_list


def time_cal(function):
    '''
    用装饰器实现函数计时
    :param function: 需要计时的函数
    :return: None
    '''
    @wraps(function)
    def function_timer(*args, **kwargs):
        t0 = time.time()
        result = function(*args, **kwargs)
        t1 = time.time()
        logging.info('[Function: {name} finished, spent time: {time:.2f}s]'.format(name = function.__name__,time = t1 - t0))
        #print '[Function: {name} finished, spent time: {time:.2f}s]'.format(name = function.__name__,time = t1 - t0)
        return result
    return function_timer


def send_wechat_warning(p_user, msg):
    """ pass
    """
    if isinstance(msg, str):
        msg = msg.decode('utf8').encode('gbk')
    elif isinstance(msg, unicode):
        msg = msg.encode('gbk')

    os.system('/usr/local/support/bin/send_weixin.pl "%s" "%s" ' % (str(p_user), msg))


time_cache = {}
def send_wechat(p_account, interval_seconds=1, ext_msg=""):
    '''
    用装饰器实现发微信
    :param function: 注册的函数
    :param p_account: p账号
    :interval_seconds: 消息发送间隔时间
    :return: None
    '''
    def log_time(func):
        def wrap_func(*args, **kwargs):
            """ pass
            """
            wrap_func.__name__ = func.__name__
            try:
                return func(*args,** kwargs)
            except Exception as e:
                key, ttime = "%s_%s" % (func.__name__, p_account), time.time()
                if ttime - time_cache.get(key, 0) > interval_seconds:
                    ip = os.popen("ifconfig | grep 'inet addr:' | grep -v '127.0.0.1' | cut -d: -f2 | awk '{print $1}' | head -1").read().replace('\n', '')
                    now_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                    info_msg = 'time=%s\n__module__=%s\nip=%s\nfunc=%s\next_msg=%s\n' % (now_time, func.__module__, ip, func.__name__, ext_msg)
                    info_msg += ',args=%s, kwargs=%s' % (args, kwargs)
                    send_wechat_warning(p_account, info_msg + ("errmsg=%s\n" % str(e)) + ("traceback=%s" % traceback.format_exc()))
                    time_cache[key] = time.time()
                raise Exception(str(e) + traceback.format_exc())
        return wrap_func
    return log_time


def get_lines(extra_file_path):
    """ 读取大文件，防止内存过大
       for example: for line in get_line(file_path):
    """
    with open(extra_file_path, 'r') as e_file:
        for index, line in enumerate(e_file):
            if line.endswith('\n'):
                yield line


def my_align(string, length=20, to_windows=False):
    """ pass
    """
    string = to_unicode(string)
    if length == 0: return string

    if to_windows:
        chinese_num, ascill_num, ch = 0, 0, unichr(12288)
        for c in to_unicode(string):
            if ord(c) > 125:
                chinese_num += 1
            else:
                ascill_num += 1

        while chinese_num % 5 != 0:
            string += ch
            chinese_num += 1

        while ascill_num + chinese_num * 1.8 < length:
            string += ' '
            ascill_num += 1
        return string
    else:
        slen = len(string)
        re = string
        for c in to_unicode(string):
            if ord(c) < 256: re += ' '
        while slen < length:
            re += '  '
            slen += 1
        return re


def str2int(string):
    """ pass
    """
    if string.endswith('w+'):
        string = string.replace('w+', '0001')
    elif string.endswith('w'):
        string = string.replace('w', '0000')
    elif string.endswith('W+'):
        string = string.replace('W+', '0001')
    elif string.endswith('W'):
        string = string.replace('W', '0000')

    if string.endswith('k+'):
        string = string.replace('k+', '001')
    elif string.endswith('k'):
        string = string.replace('k', '000')
    elif string.endswith('K+'):
        string = string.replace('K+', '001')
    elif string.endswith('K'):
        string = string.replace('K', '000')
 
    return int(string)


def bytes_to_long(byte_list):
    retVal = (byte_list[0] & 0xFF)
    for i in range(1, min(len(byte_list), 8)):
        retVal |= (byte_list[i] & 0xFFL) << (i * 8)
    return get_cbin_long(bin(retVal)[2::])


def bytes_to_int(bytes):
    result = 0
    for b in bytes:
        result = result * 256 + int(b)
    return result


def get_big(finger):
    finger = hex(finger)[2::]
    if finger.endswith('L'):
        finger = finger[0: len(finger) - 1]
    finger = finger.decode('hex')[::-1].encode('hex')
    return int(finger, 16)


def int_to_bytes(value, length):
    result = []
    for i in range(0, length):
        result.append(value >> (i * 8) & 0xff)
    result.reverse()
    return result

sys.path.append(os.path.dirname(os.path.split(os.path.realpath(__file__))[0]))
clib = ctypes.CDLL('%s/lib/clib.so' % os.path.dirname(os.path.split(os.path.realpath(__file__))[0]))
clib.get_bin_long_prx.argtypes=[c_char_p]
clib.get_bin_long_prx.restype=c_longlong
clib.mod_prx.argtypes=[c_longlong]
clib.mod_prx.restype=c_longlong
get_cbin_long=clib.get_bin_long_prx
cmod=clib.mod_prx


#print remove_brackets_info("aabb(fds)ccdd")
#print str2int('16K+')
#print str2int('16K')
#print str2int('16W')
#print str2int('16W+')


#@time_cal
#@send_wechat("p_lzhouwu", 30)
#def test(a, b):
#    return a / b

#print my_align('我的绝色总裁未婚妻', 25, True)
#print my_align('老婆大人，余生请多多指教', 25, True)
#test(1, 0)
#print get_max_same_str("放假啊看", "放假啦")
#print filter_special_char("操，你。妈了个，逼")
#print is_same('吴浪舟哇哈哈这是一个故事', '吴浪舟哇哈哈', dis_rate=0.5)
#print get_puncutation_count(",.//??、、。。。")
