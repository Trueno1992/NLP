#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os
import logging
import smtplib

from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication


sender = 'trueno_wlz@sina.com'
passwd = 'www13787196448'

sender = 'wulangzhou@yuewen.com'
passwd = 'MorePwd@159'


def send_mail(subject, content, receivers, cc_receivers=None, is_content_html=False, file_path=None, plugins=None):
    """ pass
    """
    msg = MIMEMultipart()
    msg.set_charset('utf-8')
    msg['From'] = sender
    msg['To'] = ','.join(receivers)
    if cc_receivers:
        msg['Cc'] = ','.join(cc_receivers)
    else:
        cc_receivers = []

    msg['subject'] = subject

    if is_content_html:
        msg.attach(MIMEText(content, 'html', 'utf-8'))
    else:
        msg.attach(MIMEText(content, 'plain', 'utf-8'))

    if plugins:
        for plugin in plugins:
            file_stream = MIMEApplication(plugin['content'])
            file_stream.set_charset('utf-8')
            file_stream.add_header('content-disposition', 'attachment', filename=plugin['subject'])
            msg.attach(file_stream)

    if file_path:
        file_stream = MIMEText(open(file_path, 'rb').read(), 'base64', 'utf-8')
        file_stream["Content-Type"] = 'application/octet-stream'
        file_stream["Content-Disposition"] = 'attachment; filename="%s"' % os.path.split(file_path)[1]
        msg.attach(file_stream)

    smtpObj = smtplib.SMTP()
    #smtpObj.connect('smtp.sina.com', 25)
    smtpObj.connect("smtp.exmail.qq.com", 25)

    smtpObj.login(sender, passwd)
    smtpObj.sendmail(sender, receivers + cc_receivers, msg.as_string())
    logging.info('mail is send ok')



if __name__ == '__main__':
    send_mail('python 邮件测试',
              '<p color=red>内容是乱起八糟啦</p>',
              ['wulangzhou@yuewen.com'],
              file_path='./utils.py',
              cc_receivers=['1207175256@qq.com'],
              is_content_html=True)
