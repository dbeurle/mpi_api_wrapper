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
                sh 'cat Jenkinsfile'
                sh 'ls'
                sh 'mkdir build'
                sh 'cd build'
                sh 'source /etc/profile.d/modules.sh'
                sh 'module load mpi/openmpi-x86_64'
                sh 'cmake ..'
                sh 'make all -j4'
            }
        }
    }
}
