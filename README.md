Actual logic code of matrix multilpy is in the file 'c_ectension.c'. The function is extented to python through the file 'set_up.py' whith Special modified 'CMakeList.txt'.
Run 'python set_up.py build_ext --inplace' or 'python set_up.py install' in command to build the c extension, then you can use it as the Rust module.
