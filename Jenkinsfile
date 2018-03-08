pipeline {
  agent {
    docker {
      image 'fedora:27'
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