import 'package:flutter/material.dart';
import 'package:audioplayers/audioplayers.dart';

class RingtoneSelection extends StatefulWidget {
  final int selectedRingtone;
  final Function(int) onRingtoneChanged;

  @override
  RingtoneSelection({
    required this.selectedRingtone,
    required this.onRingtoneChanged,
  });

  _RingtoneSelectionState createState() => _RingtoneSelectionState();
}

class _RingtoneSelectionState extends State<RingtoneSelection> {
  late int selectedRingtone;

  @override
  void initState() {
    super.initState();
    selectedRingtone = widget.selectedRingtone;
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        const Align(
          alignment: Alignment.topLeft,
          child: Text(
            'Alarm sound',
            style: TextStyle(fontSize: 20, color: Colors.black),
          ),
        ),
        Align(
            alignment: Alignment.topLeft,
            child: DropdownButtonExample(
              selectedRingtone: selectedRingtone,
              onRingtoneChanged: widget.onRingtoneChanged,
            )),
      ],
    );
  }
}

class DropdownButtonExample extends StatefulWidget {
  final int selectedRingtone;
  final Function(int) onRingtoneChanged;

  @override
  DropdownButtonExample({
    required this.selectedRingtone,
    required this.onRingtoneChanged,
  });

  _DropdownButtonExampleState createState() => _DropdownButtonExampleState();
}

class _DropdownButtonExampleState extends State<DropdownButtonExample> {
  late String selectedValue = 'Alarm ${widget.selectedRingtone}';
  late int selectedRingtone;
  AudioCache audioCache = AudioCache();
  AudioPlayer audioPlayer = AudioPlayer();


  @override
  void initState() {
    super.initState();
    selectedRingtone = widget.selectedRingtone;
  }

  @override
  Widget build(BuildContext context) {
    return DropdownButton<String>(
      dropdownColor: Colors.grey[200],
      value: selectedValue,
      icon: const Icon(
        Icons.arrow_drop_down,
        color: Colors.black,
        size: 25,
      ),
      onChanged: (String? newValue) async {
        setState(() {
          selectedValue = newValue!;
          selectedRingtone = int.parse(selectedValue.substring(6, 7));
          widget.onRingtoneChanged(selectedRingtone);
        });

        await audioPlayer.stop();
        await audioCache.clearAll(); // Clear the cache to prevent caching conflicts

        await audioCache.load('ringtone$selectedRingtone.mp3');
        audioPlayer = await audioCache.play('ringtone$selectedRingtone.mp3');
      },

      style: const TextStyle(fontSize: 17, color: Colors.black),
      items: const [
        DropdownMenuItem<String>(
          value: "Alarm 1",
          child: Text("Alarm 1"),
        ),
        DropdownMenuItem<String>(
          value: "Alarm 2",
          child: Text("Alarm 2"),
        ),
        DropdownMenuItem<String>(
          value: "Alarm 3",
          child: Text("Alarm 3"),
        ),
        DropdownMenuItem<String>(
          value: "Alarm 4",
          child: Text(
            "Alarm 4",
            style: TextStyle(color: Colors.black),
          ),
        ),
        DropdownMenuItem<String>(
          value: "Alarm 5",
          child: Text(
            "Alarm 5",
            style: TextStyle(color: Colors.black),
          ),
        )
      ],
    );
  }
}
