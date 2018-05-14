pipeline {
    agent {
        dockerfile {
            filename 'Dockerfile.build'
            additionalBuildArgs '--pull'
        }
    }
    options { newContainerPerStage() }
    stages {
        stage('build') {
            steps {
                sh 'if [ ! -d "build" ]; then mkdir build; fi'
                sh 'cd build'
                sh 'source /etc/profile.d/modules.sh \
                    module load mpi/openmpi-x86_64'
                sh 'cmake ..'
                sh 'make all -j4'
                sh 'ctest'
            }
        }
    }
}
