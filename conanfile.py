from conans import ConanFile


class TerseLambdaConanFile(ConanFile):
    name = 'terse_lambda'
    version = '0.1.0'
    generators = ('cmake_paths',)
    settings = ()
    requires = (
        'boost/1.68.0@conan/stable',
    )
    default_options = {'boost:header_only': True}
