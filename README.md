### Aligned Memory Allocator for NumPy

An experimental NumPy memory allocator that aligns all allocations to 64 bytes.
Written as a Python extension in C to investigate a performance regression in Arrow 12.0. 

The main source code is in `src`:
- `np_aligned_alloc.c` core implementation with no Python or Numpy dependencies
- `np_aligned_alloc_module.c` Python extension module
- `test.py` Numpy tests with the allocator enabled

## Building and Installing

In a virtual/conda environment with Numpy installed (+ pytest & hypothesis if testing): 
```shell
cd np-aligned-alloc
pip wheel --no-deps --no-build-isolation . 
pip install --no-build-isolation np_aligned_alloc-*.whl
```

## Usage
```python
import np_aligned_alloc

# Call enable to make Numpy use the aligned memory allocator
np_aligned_alloc.enable()

# Call disable to revert to the default allocator
np_aligned_alloc.disable()
```

See `src/test.py` for an example.
