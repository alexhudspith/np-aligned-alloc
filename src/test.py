#
# Numpy tests require pytest and hypothesis.
#

import np_aligned_alloc
import numpy as np


def main():
    print('Enabling np_aligned_alloc')
    np_aligned_alloc.enable()
    arr = np.arange(1000)
    print('Array address: ', arr.ctypes.data)
    print('64-byte offset:', arr.ctypes.data % 64)
    print()

    np.core.test()

    np_aligned_alloc.disable()


if __name__ == '__main__':
    main()
