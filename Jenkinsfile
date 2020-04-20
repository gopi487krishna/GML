pipeline {
    agent any
    stages {
        stage('Configure'){
            steps {
                dir('build') {
                    sh 'cmake ../'
                }
            }

        }
        stage('Build'){
            steps {
                dir('build'){
                    sh 'make'
                }
            }
        }
        stage('Test'){
            steps {
                dir('build') {
                    sh 'ctest -T test --no-compress-output'
                }
             }
        }
        stage('Deploy'){
            steps {
                sh 'git clone git://github.com/mosra/m.css'
                sh 'git clone git://github.com/gopi487krishna/gml-docs'
                sh './m.css/documentation/doxygen.py Doxyfile-mcss'
                sh 'rsync -a -delete html/ gml-docs/'
                sh 'cd gml-docs && git add . && git config user.name "gopi487krishna" && git config user.email "krishnagopi487.github@outlook.com" && git commit -m "Updated Documentation" && git remote rm origin && git remote add origin git@github.com:gopi487krishna/gml-docs.git && git push origin master'
            }
        }
    }
    post {
        always {
            // Archive the CTest xml output
            archiveArtifacts(
                artifacts: 'build/Testing/**/*.xml',
                fingerprint: true
            )
            // Process the CTest xml out with XUnit Plugin
            xunit([CTest(deleteOutputFiles: true, failIfNotNew: true, pattern: 'build/Testing/**/*.xml', skipNoTestFiles: false, stopProcessingIfError: true)])
            deleteDir()
        }
    }

}
