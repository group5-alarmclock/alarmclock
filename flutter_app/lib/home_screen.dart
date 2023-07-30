import 'dart:async';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:analog_clock/analog_clock.dart';
import 'settings_screen.dart';
import 'package:intl/intl.dart';
import 'util/snake_game.dart';
import 'alarms_screen.dart';

class HomePage extends StatefulWidget {
  HomePage({Key? key}) : super(key: key);

  @override
// ignore: library_private_types_in_public_api
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<HomePage> {
  int tapCount = 0;

  @override
  Widget build(BuildContext context) => MaterialApp(
        debugShowCheckedModeBanner: false,
        home: Scaffold(
          appBar: AppBar(
            backgroundColor: Colors.grey[300],
            elevation: 0,
            title: const Text(
              'Clock',
              style: TextStyle(color: Color(0xFF008B8f), fontSize: 22),
            ),
            actions: [
              IconButton(
                icon: const Icon(Icons.settings, color: Color(0xFF008B8f), size: 30),
                onPressed: () {
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => SettingsScreen()),
                  );
                },
              ),
            ],
          ),
          backgroundColor: Colors.grey[300],
          body: Center(
            child: Column(
              children: [
                const SizedBox(height: 50.0),
                Row(
                  children: const [
                    Text('                '),
                    DigitalClock(),
                  ],
                ),
                Row(
                  children: [
                    const Text('                '),
                    DayAndDateWidget(),
                  ],
                ),
                const SizedBox(height: 50.0),
                // Adjust the spacing between the digital clock and analog clock
                GestureDetector(
                  onTap: () {
                    tapCount++;
                    if (tapCount == 10) {
                      final dataBase = FirebaseDatabase.instance.reference();
                      final child = dataBase.child('Alarms');
                      child.update({'is alarm off': "00"});
                      tapCount = 0;
                    }
                  },
                  child: Container(
                    width: 330.0,
                    height: 350.0,
                    child: AnalogClock(
                      decoration: BoxDecoration(
                        border: Border.all(width: 2, color: Color(0xFF008B8f)),
                        color: Colors.grey[400],
                        shape: BoxShape.circle,
                      ),
                      width: 350.0,
                      isLive: true,
                      hourHandColor: Colors.black,
                      minuteHandColor: Colors.black,
                      secondHandColor: Color(0xFF008B8f),
                      showSecondHand: true,
                      numberColor: Colors.black,
                      showNumbers: true,
                      textScaleFactor: 1.5,
                      showTicks: true,
                      showDigitalClock: false,
                      digitalClockColor: Colors.black,
                      datetime: DateTime.now(),
                    ),
                  ),
                ),
              ],
            ),
          ),
        ),
      );
}

class DigitalClock extends StatefulWidget {
  const DigitalClock({Key? key}) : super(key: key);

  @override
  _DigitalClockState createState() => _DigitalClockState();
}

class _DigitalClockState extends State<DigitalClock> {
  late Timer _timer;
  late DateTime _currentTime;

  @override
  void initState() {
    super.initState();
    _currentTime = DateTime.now();
    _timer = Timer.periodic(const Duration(seconds: 1), (_) {
      setState(() {
        _currentTime = DateTime.now();
      });
    });
  }

  @override
  void dispose() {
    _timer.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final hour = _currentTime.hour.toString().padLeft(2, '0');
    final minute = _currentTime.minute.toString().padLeft(2, '0');
    final second = _currentTime.second.toString().padLeft(2, '0');

    return Text(
      '$hour:$minute',
      style: const TextStyle(
        color: Colors.black,
        fontSize: 50.0,
        fontWeight: FontWeight.w300,
      ),
    );
  }
}

class DayAndDateWidget extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    final now = DateTime.now();
    final dayOfWeek = now.weekday;
    final day = now.day;
    final month = now.month;
    final year = now.year;

    final daysOfWeek = ['Mon', 'Tue', 'Wed', 'Thurs', 'Fri', 'Sat', 'Sun'];
    final months = [
      'January',
      'February',
      'March',
      'April',
      'May',
      'June',
      'July',
      'August',
      'September',
      'October',
      'November',
      'December'
    ];

    final dayOfWeekString = daysOfWeek[dayOfWeek - 1];
    final monthString = months[month - 1];
    final dateText = '$dayOfWeekString, $monthString ';

    return Row(
      children: [
        Text(
          dateText,
          style: const TextStyle(
            fontSize: 20.0,
            color: Colors.black,
            fontWeight: FontWeight.w400,
          ),
        ),
        Text(
          '$day',
          style: const TextStyle(
            fontSize: 20.0,
            color: Color(0xFF008B8f),
            fontWeight: FontWeight.w400,
          ),
        ),
      ],
    );
  }
}

class TimezoneWidget extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    final now = DateTime.now();
    final timezone = now.timeZoneOffset;

    final timezoneString = timezone.toString();

    return Text(
      'Timezone: $timezoneString',
      style: const TextStyle(
        fontSize: 20.0,
        color: Colors.white,
        fontWeight: FontWeight.w400,
      ),
    );
  }
}

class ToggleSwitch extends StatefulWidget {
  final bool initialValue;
  final ValueChanged<bool> onChanged;

  const ToggleSwitch({
    required this.initialValue,
    required this.onChanged,
    Key? key,
  }) : super(key: key);

  @override
  _ToggleSwitchState createState() => _ToggleSwitchState();
}

class _ToggleSwitchState extends State<ToggleSwitch> {
  late bool isSwitched;

  @override
  void initState() {
    super.initState();
    isSwitched = widget.initialValue;
  }

  @override
  Widget build(BuildContext context) {
    return Switch(
      value: isSwitched,
      onChanged: (value) {
        setState(() {
          isSwitched = value;
        });
        widget.onChanged(value);
      },
      activeTrackColor: Colors.lightGreen,
      activeColor: Colors.green,
    );
  }
}
