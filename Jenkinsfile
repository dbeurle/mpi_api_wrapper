pipeline {
  agent {
    docker {
      image 'fedora:27'
      args '-v /tmp:/tmp'
    }
    
  }
  stages {
    stage('print_garbage') {
      steps {
        sh 'echo "Hello world"'
      }
    }
  }
}