pipeline {
  agent {
    docker {
      image 'fedora'
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