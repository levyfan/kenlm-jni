# kenlm-jni

Java JNI wrapper for [kenlm](https://github.com/kpu/kenlm).

## Installation

* maven
* cmake
* boost

```bash
mvn clean install
```

## Basic Usage

```java
Model model = new Model("test.arpa", new Config());
System.out.println(model.score("this is a sentence .", true, true));
model.close();
```

See [ModelTest](src/test/java/kenlm/ModelTest.java) for more, including stateful APIs.
