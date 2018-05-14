pipeline {
    agent {
        dockerfile {
            filename 'Dockerfile.build'
            additionalBuildArgs '--pull'
        }
    }
    stages {
        stage('build') {
            steps {
                sh 'source /etc/profile.d/modules.sh \
                    module load mpi/openmpi-x86_64'
                sh 'if [ ! -d "build" ]; then mkdir build; fi \
                    cd build \
                    rm -rf * \
                    cmake .. \
                    make all -j4 \
                    ctest'
            }
        }
    }
}
