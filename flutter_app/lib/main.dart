import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:app/util/snake_game.dart';
import 'package:hive_flutter/hive_flutter.dart';
import 'tasks_screen.dart';
import 'settings_screen.dart';
import 'alarms_screen.dart';
import 'timer_screen.dart';
import 'home_screen.dart';
import 'tasks_screen.dart';
import 'package:http/http.dart' as http;


void main() async {
  // Init Hive
  await Hive.initFlutter();
  await Hive.openBox('mybox');

  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();

  final dataBase = FirebaseDatabase.instance.reference();
  final child = dataBase.child('Alarms/is alarm off');

  child.onValue.listen((event) {
    if (event.snapshot.value != "10") {
      runApp(SnakeGame());
    } else {
      runApp(const MyApp());
    }
  });
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      home: const HomeScreen(),
      routes: {
        '/Alarms': (context) => AlarmsScreen(),
        '/tasks_page': (context) => TasksPage(),
        '/util/Snake': (context) => TimerPage(),
      },
      theme: ThemeData(
        bottomNavigationBarTheme: const BottomNavigationBarThemeData(
          unselectedItemColor: Colors.black,
          selectedItemColor: Color(0xFF008B8f),
        ),
      ),
    );
  }
}

void sendDataToESP32() async {
  final url = Uri.parse('http://1.1.1.1/');
  final response = await http.get(url, headers: {'message': 'Hello'});

  if (response.statusCode == 200) {
    print('Message sent successfully');
  } else {
    print('Failed to send message. Error code: ${response.statusCode}');
  }
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({Key? key});

  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  int _currentIndex = 0;

  final List<Widget> _screens = [
    HomePage(),
    AlarmsScreen(),
    TimerPage(),
    TasksPage(),
  ];

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: IndexedStack(
        index: _currentIndex,
        children: _screens,
      ),
      bottomNavigationBar: BottomNavigationBar(
        elevation: 0,
        type: BottomNavigationBarType.fixed,
        backgroundColor: Colors.grey[300],
        currentIndex: _currentIndex,
        onTap: (index) {
          setState(() {
            _currentIndex = index;
          });
        },
        items: const [
          BottomNavigationBarItem(
            icon: Icon(Icons.home_outlined),
            label: 'clock',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.alarm_outlined),
            label: 'Alarm',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.timer_outlined),
            label: 'Study',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.task_outlined),
            label: 'Tasks',
          ),
        ],
      ),
    );
  }
}
