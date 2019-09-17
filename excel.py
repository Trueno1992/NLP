#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import xlrd
import xlwt
import random
import string
import datetime
from cStringIO import StringIO

from lib.utils import to_str


class Excel(object):
    ''' excel processing
    '''

    def __init__(self, home='/tmp', trans=None):
        """ pass
        """
        self.home = home
        self.trans = trans

    def get_title_style(self):
        """ excel title style
        """
        style = xlwt.XFStyle()
        font = xlwt.Font()
        font.bold = True
        style.font = font
        alignment = xlwt.Alignment()
        alignment.horz = xlwt.Alignment.HORZ_CENTER
        alignment.vert = xlwt.Alignment.VERT_CENTER
        style.alignment = alignment
        return style

    def get_text_style(self):
        """ excel text style
        """
        style = xlwt.XFStyle()
        alignment = xlwt.Alignment()
        alignment.horz = xlwt.Alignment.HORZ_CENTER
        alignment.vert = xlwt.Alignment.VERT_CENTER
        style.alignment = alignment
        return style

    def _get_date(self, vtime, dateformat="%Y-%m-%d"):
        """ excel日期处理
        """
        if isinstance(vtime, float):
            vtime = int(vtime)
        s_date = datetime.date(1899, 12, 31).toordinal() - 1
        d = datetime.date.fromordinal(s_date + vtime)
        return d.strftime(dateformat)

    def load_file(self, file_name, dateformat="%Y-%m-%d"):
        ''' assume the first line is title
        '''
        work_book = xlrd.open_workbook(file_name)
        sheet = work_book.sheet_by_index(0)

        title = []
        for c in xrange(sheet.ncols):
            title.append(sheet.cell(0, c).value)

        data = []
        for r in xrange(1, sheet.nrows):
            unit = {}
            for c in xrange(sheet.ncols):
                cell = sheet.cell(r, c)
                if cell.ctype == xlrd.XL_CELL_DATE:
                    unit[title[c]] = to_str(self._get_date(cell.value, dateformat))
                else:
                    unit[title[c]] = to_str(cell.value)
            data.append(unit)
        return data

    def tmp_filename(self):
        """ pass
        """
        return os.path.join(self.home, ''.join(random.sample(string.lowercase, 10)))

    def load_body(self, body=None):
        """ pass
        """
        # 如果没有文件,则根据body写入文件
        if body is None:
            raise Exception("No body")

        file_name = self.tmp_filename()
        try:
            with open(file_name, 'w+b') as codefp:
                codefp.write(body)

            work_book = xlrd.open_workbook(file_name)
            sheet = work_book.sheet_by_index(0)

            title = []
            for c in xrange(sheet.ncols):
                title.append(sheet.cell(0, c).value)

            data = []
            for r in xrange(1, sheet.nrows):
                unit = {}
                for c in xrange(sheet.ncols):
                    cell = sheet.cell(r, c)
                    if cell.ctype == xlrd.XL_CELL_DATE:
                        unit[title[c]] = to_str(self._get_date(cell.value, dateformat))
                    else:
                        unit[title[c]] = to_str(cell.value)
                data.append(unit)
            return data
        finally:
            os.remove(file_name)

    def trans_to_name(self, key, value):
        """ pass
        """
        result = value
        # 对问题类型特殊处理
        if key == "err_type":
            book_error_dict = self.trans[key]
            if value:
                error_types = [ x for x in value.split(",") if x]
                result = "错误类型如下:"
                for x in xrange(len(error_types)):
                    result = "%s%s:%s;" % (result, x+1, book_error_dict.get(error_types[x], ""))
            return result

        if self.trans and self.trans.get(key, None):
            trans_dict = self.trans[key]
            return trans_dict.get(str(value), value)

        return result

    def generate(self, title, data, sep=1, callback=None, time_format='%Y-%m-%d'):
        """ generate excel content
        """
        work_book =  xlwt.Workbook('UTF-8')
        sheet = work_book.add_sheet('sheet', True)

        ncols = len(title)
        title_style = self.get_title_style()
        for j in xrange(ncols):
            sheet.col(j).width = 3333 * sep
            sheet.write(0, j, title[j][1], title_style)

        nrows = len(data)
        text_style = self.get_text_style()
        for i in xrange(nrows):
            for j in xrange(ncols):
                key = title[j][0]
                val = callback(data[i], key) if callback else data[i].get(key,'')
                val = self.trans_to_name(key, val)
                if isinstance(val, datetime.datetime):
                    val = val.strftime(time_format)
                sheet.write(i+1, j, val, text_style)

        output = StringIO()
        work_book.save(output)
        output.seek(0)
        return output.read()

    def write_records(self, title, data, save_path=None, to_path=None, sep=1, callback=None, time_format='%Y-%m-%d'):
        """ generate excel content
        """
        if not save_path and not to_path:
            raise Exception('not have save_path or to_path')

        if save_path and to_path:
            raise Exception("can't exist have save_path and to_path at same time")

        work_book =  xlwt.Workbook('UTF-8')
        sheet = work_book.add_sheet('sheet', cell_overwrite_ok=True)

        ncols = len(title)
        title_style = self.get_title_style()
        for j in xrange(ncols):
            sheet.col(j).width = 3333 * sep
            sheet.write(0, j, title[j][1], title_style)

        if to_path:
            if os.path.exists(to_path):
                records = self.load_file(to_path)
                for record in records:
                    items = {}
                    for eng, chinese in title:
                        if isinstance(chinese, str):
                            chinese = chinese.decode('utf-8')
                        items[eng] = record.get(chinese)
                    data.append(items)
            save_path = to_path

        nrows = len(data)
        text_style = self.get_text_style()
        for i in xrange(nrows):
            for j in xrange(ncols):
                key = title[j][0]
                val = callback(data[i], key) if callback else data[i].get(key,'')
                val = self.trans_to_name(key, val)
                if isinstance(val, datetime.datetime):
                    val = val.strftime(time_format)
                sheet.write(i+1, j, val, text_style)
        work_book.save(save_path)

    @staticmethod
    def look_excel(save_path):
        """ pass
        """
        records = Excel().load_file(save_path)
        for record in records:
            for k, v in record.items():
                print k, v
            print ''


def test1():
    """ pass
    """
    title = [('name', '姓名'), ('age', '年龄'), ('gender', '性别')]

    data = [{'name': 'liangsix', 'age': 10,'gender': '男'},
            {'name': '梁六', 'age': 10, 'gender': '男'}]

    with open('./test1.xls','wb') as f:
        f.write(Excel().generate(title, data))


def test2():
    """ pass
    """
    title = [('name', '姓名'), ('age', '年龄'), ('gender', '性别')]

    data = [{'name': 'liangsix', 'age': 11, 'gender': '男'},
            {'name': '梁六', 'age': 11, 'gender': '男'}]

    Excel().write_records(title, data, to_path='./test2.xls')


def test3():
    """ pass
    """
    records = Excel().load_file('./test2.xls')
    for record in records:
        for k, v in record.items():
            print k, v
        print ''


if __name__ == '__main__':
    sys.path.append(os.path.dirname(os.path.split(os.path.realpath(__file__))[0]))
    test1()
    test2()
    test3()
