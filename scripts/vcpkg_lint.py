import os

if not os.environ.get('VCPKG_ROOT'):
    print('cannot find vcpkg')
else:
    vcpkg = os.environ['VCPKG_ROOT']
    os.system(f'{vcpkg}/vcpkg format-manifest vcpkg.json')
