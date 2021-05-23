# Supported Package Managers

 * [DDS][dds]


# DDS

TerseLambda can be used by [DDS](https://github.com/vector-of-bool/dds).
A pre-made DDS repo for TerseLambda can be found at https://github.com/Quincunx271/dds-repos;
these steps assume you wish to use that repo:

  * Add the repo to DDS:

    ```bash
    dds pkg repo add "https://quincunx271.github.io/dds-repos/public"
    ```

  * Tell DDS that you want to use the package:

    ```json5
      // In your package.json5
      depends: [
        'terse-lambda@0.2.0'
      ]
      // In your library.json5
      uses: [
        'quincunx271/terse-lambda'
      ]
    ```

  [dds]: #dds
