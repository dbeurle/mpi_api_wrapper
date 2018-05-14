pipeline {
  agent none
  stages {
    stage('build') {
      agent {
       dockerfile {
         filename 'Dockerfile.build'
         additionalBuildArgs '--pull'
       }
      }
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
  post {
    failure {
        mail to: 'peter@beurle.id.au',
             subject: "Failed Pipeline still not working :( : ${currentBuild.fullDisplayName}",
             body: "Something is wrong with ${env.BUILD_URL}"
    }
  }
}
