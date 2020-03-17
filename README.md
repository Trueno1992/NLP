suggest.py 实现了基于权重的前缀联想，支持各种api查询

示例：
 from suggest import Tree
 root = Tree()
 root.insert('叶非夜', 20, [])    # 字符串，权重，附加信息
 root.insert('叶非非', 10, [])
 root.get_suffix('叶', n)         # 获取以“叶”开头的所有后缀字符串，取top_n（最坏情况时间复杂度 top_n * top_n * 字符长度）
 root.get_suffix_count(args)      # 只获取数量
 root.get_lcp_suffix('叶非天', n) # 获取最长公共字符的所有后缀，取top_n
 root.get_lcp_suffix_count(args) # 只获取数量
