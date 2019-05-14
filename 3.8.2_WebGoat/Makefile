make:
	mvn package -f WebGoat-Lessons-develop/pom.xml
	cp WebGoat-Lessons-develop/target/plugins/*.jar WebGoat-7.1/webgoat-container/src/main/webapp/plugin_lessons/

clean:
	mvn clean -f WebGoat-Lessons-develop/pom.xml
	rm -rf WebGoat-7.1/webgoat-container/src/main/webapp/plugin_lessons/*.jar

server:
	cd WebGoat-7.1 && java -jar webgoat-standalone/target/webgoat-standalone-7.1-exec.jar --port 8888