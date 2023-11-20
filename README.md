### Aligned Memory Allocator for NumPy

An experimental NumPy memory allocator that aligns all allocations to 64 bytes.
Written as a Python extension in C to investigate a performance regression in Arrow 12.0
(https://github.com/apache/arrow/issues/35498, since fixed).

The main source code is in `src`:
- `np_aligned_alloc.c` core implementation with no Python or Numpy dependencies
- `np_aligned_alloc_module.c` Python extension module
- `test.py` Numpy tests with the allocator enabled

## Installing and Running Tests 

The Numpy tests require PyTest and Hypothesis.

Either using venv:
```shell
cd np-aligned-alloc
python3.10 -m venv ./venv
source ./venv/bin/activate
pip install .[test]
python src/test.py
```

Or conda:
```shell
cd np-aligned-alloc
conda env create -f ./environment.yml
conda activate np-aligned-alloc
python src/test.py
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
