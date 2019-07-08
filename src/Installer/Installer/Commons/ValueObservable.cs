using System;
using System.Collections.Generic;

namespace Installer.Commons
{
    public class ValueObservable<T>
    {
        public T Value
        {
            get { return _value; }
            set { Change(value); }
        }

        readonly List<Action<T>> _actionList = new List<Action<T>>();
        private T _value;

        public ValueObservable(T initialValue)
        {
            Value = initialValue;
        }

        public void AddObserver(Action<T> action)
        {
            _actionList.Add(action);
            action(Value);
        }

        public void Change(T newValue)
        {
            if(Value!=null && Value.Equals(newValue))
            {
                return;
            }

            _value = newValue;
            foreach (var action in _actionList)
            {
                action(newValue);
            }
        }
    }
}