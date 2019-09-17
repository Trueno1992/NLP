# NLP
nlp frequetly useful module function
edit.cpp 和  Levenshtein.cpp 都是编辑距离的实现文件。虽然python的python-Levenshtein 已经实现了编辑距离，但很多时候我们不仅需要编辑距离，还需要编辑
所对应的编辑轨迹。所以自己实现了一遍，get_edit_path 可以拿到编辑距离和对应的编辑轨迹，get_edit_dis 可以获取编辑距离，setup.py 可以文件可以在支持python
用disutils 安装，安装命令 python setup.py install。可以参考edit_test.py进行使用
